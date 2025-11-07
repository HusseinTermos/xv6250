#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

#define MAX_LINE_LEN 10
#define MAX_LINES 10
  
int main(int argc, char *argv[])
{
  if (argc < 3) {
    printf(2, "Usage: %s file_name1 file_name2\n", argv[0]);
    exit();
  }
  int f1 = open(argv[1], O_RDONLY);
  int f2 = open(argv[2], O_RDONLY);
  if (f1 < 0) {
    printf(2, "Error: could not open file\n");
  }
  if (f2 < 0) {
    printf(2, "Error: could not open file 2\n");
    exit();
  }
  char f1_data[MAX_LINES][MAX_LINE_LEN];
  char f2_data[MAX_LINES][MAX_LINE_LEN];
  int f1_len = 0, f2_len = 0;
  while (f1_len < MAX_LINES && gets_fd(f1, f1_data[f1_len], MAX_LINE_LEN)) f1_len++;
  while (f2_len < MAX_LINES && gets_fd(f2, f2_data[f2_len], MAX_LINE_LEN)) f2_len++;
  
  close(f1); close(f2);
  int dp[MAX_LINES][MAX_LINES];
  dp[0][0] = !strcmp(f1_data[0], f2_data[0]) ? 1 : 0;
  for (int i = 1; i < f1_len; ++i) {
    dp[i][0] = (dp[i-1][0] || !strcmp(f1_data[i], f2_data[0])) ? 1 : 0;
  }
  for (int i = 1; i < f2_len; ++i) {
    dp[0][i] = (dp[0][i-1] || !strcmp(f1_data[0], f2_data[i])) ? 1 : 0;
  }
  for (int i = 1; i < f1_len; ++i) {
    for (int j = 1; j < f2_len; ++j) {
      if (dp[i-1][j] > dp[i][j-1]) {
        dp[i][j] = dp[i-1][j];
      }
      else {
        dp[i][j] = dp[i][j-1];
      }
      if (!strcmp(f1_data[i], f2_data[j])) {
        dp[i][j] = 1 + dp[i-1][j-1];
      }
    }
  }
  int curi = f1_len - 1, curj = f2_len - 1;
  int diag[MAX_LINES+2][2];
  int diag_len = dp[f1_len - 1][f2_len - 1] + 2;
  int curd = diag_len - 2;
  diag[diag_len - 1][0] = f1_len;
  diag[diag_len - 1][1] = f2_len;


  while (curi - 1 >= 0 && curj - 1 >= 0) {

    if (dp[curi - 1][curj] == dp[curi][curj]) {
      curi--;
    }
    else if (dp[curi][curj - 1] == dp[curi][curj]) curj--;
    else {
      diag[curd][0] = curi--;
      diag[curd--][1] = curj--; // proven
    }
  }
  diag[curd][0] = curi;
  diag[curd][1] = curj;
  while (curi >= 0 && dp[curi][0] != 0) diag[curd][0] = curi--;
  while (curj >= 0 && dp[0][curj] != 0) diag[curd][1] = curj--;

  diag[0][0] = -1;
  diag[0][1] = -1;

  for (int i = 0; i < diag_len - 1; i++) {
    for (int j = diag[i][0] + 1; j < diag[i+1][0]; j++) {
      printf(1, "- %s", f1_data[j]);
    }
    for (int j = diag[i][1] + 1; j < diag[i+1][1]; j++) {
      printf(1, "+ %s", f2_data[j]);
    }
  }

  exit();

}
