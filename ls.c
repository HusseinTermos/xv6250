#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#define NULL 0
int count_stars(char* s) {
  int count = 0, n = strlen(s);
  for(int i = 0; i < n; ++i)
    if(s[i] == '*')
      count++;
  return count;
}

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

int match(char *pattern, char *s) { // Assumes no consecutive *'s
  int m = strlen(pattern), n = strlen(s), stars = count_stars(pattern);
  if(stars == 0) {
    if(n != m) return 0;
    for(int i = 0; i < n; i++)
      if(pattern[i] != s[i]) return 0;
    return 1;
  }
  if(m == 1) return 1; // pattern is '*'
  // don't count empty string as a part
  int num_parts = stars + 1;
  if(pattern[0] == '*') num_parts--;
  if(pattern[m - 1] == '*') num_parts--;
  char** parts = (char**)malloc(num_parts * sizeof(char *));
  int* part_size = (int*)malloc(num_parts * sizeof(int));
  int i = (pattern[0] == '*') ? 1 : 0, part_index = 0;
  while(i < m) {
    int j = i;
    while(j < m && pattern[j] != '*') j++;
    j--;
    // s[i, j] is a part
    parts[part_index] = (char*)malloc((j - i + 2) * sizeof(char));
    parts[part_index][j - i + 1] = 0; // s[length] should be the null terminator
    part_size[part_index] = j - i + 1;
    for(int k = i; k <= j; ++k) parts[part_index][k - i] = pattern[k];
    part_index++;
    i = j + 2;
  }
  int p1 = 0, p2 = 0;
  // match prefix
  if(pattern[0] != '*') {
    int len = strlen(parts[0]);
    if(n < len) return 0;
    for(int j = 0; j < len; ++j)   
        if(s[j] != pattern[j]) return 0;
    m -= len;
    n -= len;
    s += len;
    pattern += len;
    p2 = 1; // don't match that prefix part
  }
  // match suffix
  if(pattern[m - 1] != '*') {
    int len = strlen(parts[num_parts - 1]);
    if(n < len) return 0;
    for(int j = 0; j < len; j++)
        if(s[n - 1 - j] != pattern[m - 1 - j]) return 0;
    n -= len;
    m -= len;
    num_parts--; // don't match that suffix part
  }
  while(p1 < n && p2 < num_parts && p1 + part_size[p2] <= n) {
    int fits = 1;
    for(int j = 0; j < part_size[p2]; ++j)
      if(s[p1 + j] != parts[p2][j]) {
        fits = 0;
        break;
      }
    if(fits) {
      p1 += part_size[p2];
      p2++;
    }
    else p1++;
  }
  // freeing up memory
  for(int j = 0; j < num_parts; ++j) {
    free(parts[j]);
    parts[j] = NULL;
  }
  free(parts);
  parts = NULL;
  free(part_size);
  part_size = NULL;
  return p2 >= num_parts;
}

void ls(char *path); // forward declaration

// Recursive function
void search(int idx, int n, char *curr_path, struct dirent* de, struct stat* st, char** parts) {
  int fd;
  if(idx == n) {
    // Last part, call normal ls on matching files
    ls(curr_path);
    return;
  }

  if((fd = open(curr_path, 0)) < 0) return;
  if(fstat(fd, st) < 0) { close(fd); return; }
  if(st->type != T_DIR) { close(fd); return; }

  char newbuf[512];
  strcpy(newbuf, curr_path);
  int len = strlen(newbuf);
  if(len > 0 && newbuf[len-1] != '/') newbuf[len++] = '/';

  while(read(fd, de, sizeof(*de)) == sizeof(*de)){
    if(de->inum == 0) continue;
    char namebuf[DIRSIZ + 1];
    memmove(namebuf, de->name, DIRSIZ);
    namebuf[DIRSIZ] = '\0';  // ensures it's null terminated

    if(!match(parts[idx], namebuf)) continue;

    strcpy(newbuf + len, namebuf); // append the name to the end of the current directory
    newbuf[len + strlen(namebuf)] = 0; // add null terminator to signal that it's the end of the path

    search(idx+1, n, newbuf, de, st, parts);
  }
  close(fd);
}

void ls_wildcard(char *path)
{
  char buf[512], *p;
  int num_parts = 1, path_length = strlen(path);
  for(int i = 0; i < path_length; i++)
    if(path[i] == '/')
      num_parts++;
  char** parts = (char**)malloc(num_parts * sizeof(char *));
  int n = 0;
  struct dirent* de = (struct dirent*)malloc(sizeof(struct dirent));
  struct stat* st = (struct stat*)malloc(sizeof(struct stat));
  for(int i = 0; i < DIRSIZ; i++)
    de->name[i] = 0; // fill with null terminators, so when file name is read, there is a terminator in the right position

  // Split path into components
  char *s = path;
  while(*s) {
    parts[n++] = s;
    while(*s && *s != '/') s++;
    if(*s) *s++ = 0; // null terminate each part
  }

  // Start recursion from first part
  if(parts[0][0] == 0) return;
  if(parts[0][0] == '/') search(0, n, "/", de, st, parts);
  else search(0, n, ".", de, st, parts);
  // free up memory
  free(parts);
  parts = NULL;
  free(de);
  de = NULL;
  free(st);
  st = NULL;
}

void ls(char *path)
{
  // Check if path contains '*'
  if(count_stars(path)) {
    ls_wildcard(path);
    return;
  }

  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
      printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit();
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit();
}
