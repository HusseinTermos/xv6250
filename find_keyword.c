#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

#define BUF_SIZE 512
#define WORD_MAX 64

// Checks for separators
int is_seperator(char c) { return c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\v' || c == '\f'; }

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf(2, "Usage: %s file keyword\n", argv[0]);
    exit();
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd < 0) {
    printf(2, "Error: could not open file %s\n", argv[1]);
    exit();
  }

  char* keyword = argv[2];
  char buf[BUF_SIZE];
  int n;
  char word[WORD_MAX];
  int wlen = 0;
  int count = 0;

  while ((n = read(fd, buf, BUF_SIZE)) > 0) {
    for (int i = 0; i < n; i++) {
      char c = buf[i];

      if (!is_seperator(c)) word[wlen++] = c;
      else {
        // at a separator, finish word
        if (wlen > 0) {
          word[wlen] = '\0';
          if (strcmp(word, keyword) == 0) count++;
          wlen = 0;
        }
      }
    }
  }

  // If last chunk ended in a word
  if (wlen > 0) {
    word[wlen] = '\0';
    if (strcmp(word, keyword) == 0) count++;
  }

  close(fd);
  printf(1, "Occurrences of \"%s\": %d\n", keyword, count);
  exit();
}
