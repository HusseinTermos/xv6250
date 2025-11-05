//A Simple C program
#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"

#define MAX_LINE_LEN 10
#define MAX_LINES 10
// o1 = dp[i-1][j] o2 = dp[i][j-1] o3 = dp[i-1][j-1]
  
int main(int argc, char *argv[])
{
  if (argc < 3) {
    printf(2, "Usage: %s file_name1 file_name2\n", argv[0]);
    exit();
  }
  int f1 = open(argv[1], O_RDONLY);
  int f2 = open(argv[2], O_RDONLY);
  if (f1 < 0 || f2 < 0) {
    printf(2, "Error: could not open file\n");
  }
  char f1_data[MAX_LINES][MAX_LINE_LEN];
  char f2_data[MAX_LINES][MAX_LINE_LEN];
  int f1_len = 0, f2_len = 0;
  while (f1_len < MAX_LINES && gets_fd(f1, f1_data[f1_len], MAX_LINE_LEN)) f1_len++;
  while (f2_len < MAX_LINES && gets_fd(f2, f2_data[f2_len], MAX_LINE_LEN)) f2_len++;
  
  close(f1); close(f2);
  if (f1_len == 0 || f2_len == 0) {
    // print all of other file
    printf(1, "OK");
    exit();
  }
  int dp[MAX_LINES][MAX_LINES];
  dp[0][0] = !strcmp(f1_data[0], f2_data[0]) ? 1 : 0;
  for (int i = 1; i < f1_len; ++i) {
    dp[i][0] = (dp[i-1][0] || !strcmp(f1_data[i], f2_data[0])) ? 1 : 0;
  }
  for (int i = 1; i < f2_len; ++i) {
    dp[0][i] = (dp[0][i-1] || !strcmp(f1_data[0], f2_data[i])) ? 1 : 0;
  }
  // int decisions[MAX_LINES][MAX_LINES];
  for (int i = 1; i < f1_len; ++i) {
    for (int j = 1; j < f2_len; ++j) {
      if (dp[i-1][j] > dp[i][j-1]) {
        dp[i][j] = dp[i-1][j];
        // decisions[i][j] = 0;
      }
      else {
        dp[i][j] = dp[i][j-1];
        // decisions[i][j] = 1;
      }
      if (!strcmp(f1_data[i], f2_data[j])) {
        if (dp[i-1][j-1] + 1 > dp[i][j]) {
          dp[i][j] = 1 + dp[i-1][j-1];
          // decisions[i][j] = 2;
        }
      }
    }
  }
  int curi = 0, curj = 0;

  while (1) {
    if (curi == f1_len - 1 && curj == f2_len - 1)
      break;
    int best = dp[curi][curj];
    int nexti = curi, nextj = curj;
    if (curi + 1 < f1_len && dp[curi + 1][curj] >= best) {
      best = dp[curi + 1][curj];
      nexti = curi + 1;
      nextj = curj;
    }

    if (curj + 1 < f2_len && dp[curi][curj + 1] >= best) {
      best = dp[curi][curj + 1];
      nexti = curi;
      nextj = curj + 1;
    }

    if (curi + 1 < f1_len && curj + 1 < f2_len && dp[curi + 1][curj + 1] >= best) {
      best = dp[curi + 1][curj + 1];
      nexti = curi + 1;
      nextj = curj + 1;
    }
    
    if (nexti == curi && nextj == curj)
      break;

    if (best == dp[curi + 1][curj]) {
      printf(1, "- %s\n", f1_data[curi]);
    }
    else if (best == dp[curi][curj+ 1]) {
      printf(1, "+ %s\n", f2_data[curj]);
    }
    else {
      printf(1, "%s\n", f1_data[curi]);
    }


    curi = nexti;
    curj = nextj;
  }


  printf(1, "LCS: %d\n", dp[f1_len - 1][f2_len - 1]);
  exit();

}
