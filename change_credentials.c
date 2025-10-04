#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


unsigned int djb2(const char *str) {
  unsigned int h = 5381;
  int c;
  while ((c = *str++))
    h = ((h << 5) + h) + c;
  return h;
}

int main(int argc, char *argv[])
{
  if (argc != 5) {
    printf(1, "Usage: change_credentials CUR_USERNAME CUR_PASSWORD NEW_USERNAME NEW_PASSWORD");
    exit();
  }
  char true_username_buf[128];
  char true_password_buf[128];
  int credentials_file = open("credentials.txt", O_RDONLY);
  if (credentials_file < 0) {
    exit();
  }
  int len_username = gets_fd(credentials_file, true_username_buf, sizeof(true_username_buf));
  int len_pass = gets_fd(credentials_file, true_password_buf, sizeof(true_password_buf));
  close(credentials_file);
  if (len_username == 0 || len_pass == 0) {
    exit();
  }
  true_username_buf[len_username-1] = '\0';
  true_password_buf[len_pass-1] = '\0';
  unsigned int true_username_hash = atoi(true_username_buf);
  unsigned int true_password_hash = atoi(true_password_buf);
  
  if(!(djb2(argv[1]) == true_username_hash && djb2(argv[2]) == true_password_hash)) {
    printf(1, "Incorrect current credentials.");
    exit();
  }
  credentials_file = open("credentials.txt", O_WRONLY);
  printf(credentials_file, "%d\n%d\n", djb2(argv[3]), djb2(argv[4]));
  printf(1, "Successfully updated username and password\n");
  exit();
}
