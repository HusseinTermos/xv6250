#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

#define MAX_LINES 20
#define MAX_LINE_LEN 64
#define BUF_SZ 32
#define MAX_FILE_NAME 40

int ceildiv(int x, int y) {
  return (x + y - 1) / y;
}


int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf(2, "Usage: %s file_name size_per_sub_file\n", argv[0]);
    exit();
  }
  int size_per_sub_file = atoi(argv[2]);
  int f1 = open(argv[1], O_RDONLY);
  if (f1 < 0) {
    printf(2, "Error: could not open file\n");
    exit();
  }
  struct stat st;
  fstat(f1, &st);
  int num_files = ceildiv(st.size, size_per_sub_file);
  char buf[BUF_SZ];
  for (int i = 1; i <= num_files; i++) {
    char file_name[MAX_FILE_NAME];
    int len = strlen(argv[1]);
    int last_d = len;
    for (int j = 0; j < len; j++) {
        if (argv[1][j] == '.') {
            last_d = j;
        }
    }
    for (int j = 0; j < last_d; j++) {
        file_name[j] = argv[1][j];
    }
    // int idx = last_d - 1;
    int num_digits = 0; int c = i; while(c > 0) {num_digits++; c/=10;}
    c = i;
    for (int j = 0; j < num_digits; j++) {
        file_name[last_d + num_digits - j - 1] = '0' + (c % 10);
        c /= 10;
    }
    file_name[last_d+num_digits] = '.';
    // idx = last_d+num_digits + 1;
    int extln=0;
    for (int j = last_d+1; j < strlen(argv[1]); j++) {
        file_name[last_d+num_digits+1 + extln] = argv[1][j];
        extln++;
    }
    file_name[last_d+num_digits+extln+1] = 0;
    int f2 = open(file_name, O_CREATE | O_WRONLY | O_TRUNC);
    int cur = 0;
    while (cur < size_per_sub_file) {
        int rem = size_per_sub_file - cur;
        int readd = read(f1, buf, min(BUF_SZ, rem));
        if (readd <= 0) break;
        write(f2, buf, readd);
        cur += readd;

    }
    close(f2);
  }
//   char f1_data[MAX_LINES][MAX_LINE_LEN];
//   int file_size = 0;
//   int f1_len = 0;
//   while (f1_len < MAX_LINES && gets_fd(f1, f1_data[f1_len], MAX_LINE_LEN)) {
//     file_size += strlen(f1_data[f1_len]);
//    // printf(1, "AA %s\n", f1_data[f1_len]);
//     f1_len++;
//   }
  close(f1);

//   int cur = 0;
//   printf(1, "%d %d FEWFFWE\n", file_size, size_per_sub_file);
//   int num_files = ceildiv(file_size,size_per_sub_file);
  printf(1, "%d FEWFFWE\n", num_files);
  exit();
}
