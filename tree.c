#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#define NULL 0

// Recursive function
void search(int depth, char *curr_path, struct dirent* de, struct stat* st) {
    int fd;
    // print here
    for(int i = 0; i < depth; ++i) {
        if(i == depth - 1) printf(1, "+---");
        else printf(1, "|   ");
    }
    int len = strlen(curr_path), s;
    for(s = len - 1; s >= 0 && curr_path[s] != '/'; --s); // find start of last element
    printf(1, "%s\n", curr_path + s + 1);

    if((fd = open(curr_path, 0)) < 0) return;
    if(fstat(fd, st) < 0) { close(fd); return; }
    if(st->type != T_DIR) { close(fd); return; }

    char newbuf[512];
    strcpy(newbuf, curr_path);
    // int len = strlen(newbuf);
    if(len > 0 && newbuf[len-1] != '/') newbuf[len++] = '/';

    while(read(fd, de, sizeof(*de)) == sizeof(*de)){
        if(de->inum == 0) continue;
        char namebuf[DIRSIZ + 1];
        memmove(namebuf, de->name, DIRSIZ);
        namebuf[DIRSIZ] = '\0';  // ensures it's null terminated

        strcpy(newbuf + len, namebuf); // append the name to the end of the current directory
        newbuf[len + strlen(namebuf)] = 0; // add null terminator to signal that it's the end of the path

        search(depth + 1, newbuf, de, st);
    }
    close(fd);
}



int
main(void)
{
    struct dirent* de = (struct dirent*)malloc(sizeof(struct dirent));
    struct stat* st = (struct stat*)malloc(sizeof(struct stat));
    for(int i = 0; i < DIRSIZ; i++)
        de->name[i] = 0; // fill with null terminators, so when file name is read, there is a terminator in the right position

    // Start recursion from first part
    search(0, ".", de, st);
    // free up memory
    free(de);
    de = NULL;
    free(st);
    st = NULL;
}
