#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

#define BUF_SZ 10
#define MAX_FILE_NAME 10

int ceildiv(int x, int y) {
  return (x + y - 1) / y;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf(2, "Usage: %s file_name size_per_sub_file\n", argv[0]);
    exit();
  }

  char file_name[MAX_FILE_NAME];
  memset(file_name, 0, sizeof(file_name));
  printf(1, "GOT HERE\n");

  int size_per_sub_file = atoi(argv[2]);

  int f1 = open(argv[1], O_RDONLY);
  if (f1 < 0) {
    printf(2, "Error: cannot open file\n");
    exit();
  }

  struct stat st;
  fstat(f1, &st);

  int num_files = ceildiv(st.size, size_per_sub_file);
  char buf[BUF_SZ];

  for (int i = 1; i <= num_files; i++) {

    // build filename
    memset(file_name, 0, sizeof(file_name));

    int len = strlen(argv[1]);
    int lastdot = len;
    for (int j = 0; j < len; j++)
      if (argv[1][j] == '.') lastdot = j;

    // copy base
    for (int j = 0; j < lastdot; j++)
      file_name[j] = argv[1][j];

    // append number
    int pos = lastdot;
    int x = i, digits = 0;
    while (x > 0) { digits++; x /= 10; }
    x = i;
    for (int d = digits-1; d >= 0; d--) {
      file_name[pos+d] = '0' + (x % 10);
      x /= 10;
    }
    pos += digits;

    // append extension ONLY if original had one
    if (lastdot < len) {
      file_name[pos++] = '.';
      for (int j = lastdot+1; j < len; j++)
        file_name[pos++] = argv[1][j];
    }
    file_name[pos] = 0;

    int f2 = open(file_name, O_CREATE | O_WRONLY);

    int written = 0;
    while (written < size_per_sub_file) {
      int rem = size_per_sub_file - written;
      int chunk = rem < BUF_SZ ? rem : BUF_SZ;
      int r = read(f1, buf, chunk);
      if (r <= 0) break;
      write(f2, buf, r);
      written += r;
    }

    close(f2);
  }

  close(f1);
  printf(1, "%d files created\n", num_files);
  // printf(1, "HELLO");
  exit();
}
