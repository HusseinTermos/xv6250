#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#define NULL 0

// Recursive function
void search(int depth, char *path) {
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

    // print indentation
    for(int i = 0; i < depth; ++i) {
        if(i == depth - 1) printf(1, "+---");
        else printf(1, "|   ");
    }
    int len = strlen(path), s;
    for(s = len - 1; s >= 0 && path[s] != '/'; --s); // find start of last element
    printf(1, "%s\n", path + s + 1);

    // only recurse if directory
    if (st.type != T_DIR) {
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

        search(depth + 1, buf);
    }

    close(fd);
}

int
main(void)
{
    // Start recursion from first part
    search(0, ".");
    exit();
}
