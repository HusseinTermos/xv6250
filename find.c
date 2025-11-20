#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#define NULL 0

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
  int num_parts = stars;
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

// Recursive function
void search(char *path, char* pattern) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
        return;
    if (fstat(fd, &st) < 0) {
        close(fd);
        return;
    }

    // only recurse if directory
    if (st.type != T_DIR) {
        // if the name matches, print the path
        int len = strlen(path), s;
        for(s = len - 1; s >= 0 && path[s] != '/'; --s); // find start of last element
        char* filename = path + s + 1;
        if(match(pattern, filename)) 
            printf(1, "%s\n", path);
        close(fd);
        return;
    }

    // prepare buffer for subpaths
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p = '/';
    p++;

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) continue;
        if (!strcmp(de.name, ".") || !strcmp(de.name, "..")) continue;

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        search(buf, pattern);
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    // Start recursion from first part
    if(argc != 2) {
        printf(1, "Use: find <pattern>\n");
        exit();
    }
    char* pattern = argv[1];
    search(".", pattern);
    exit();
}
