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
  char true_username_buf[128];
  char true_password_buf[128];
  int credentials_file = open("credentials.txt", O_RDONLY);
  if (credentials_file < 0) {
    exit();
  }
  int len_username = gets_fd(credentials_file, true_username_buf, sizeof(true_username_buf));
  int len_pass = gets_fd(credentials_file, true_password_buf, sizeof(true_password_buf));
  if (len_username == 0 || len_pass == 0) {
    exit();
  }
  true_username_buf[len_username-1] = '\0';
  true_password_buf[len_pass-1] = '\0';
  unsigned int true_username_hash = atoi(true_username_buf);
  unsigned int true_password_hash = atoi(true_password_buf);

  char username_buf[128];
  char password_buf[128];
  while (1) {

    int username_len = 0;
    while (username_len == 0) {
      printf(1, "Username: ");
      username_len = gets_fd(0, username_buf, sizeof(username_buf) - 1);
    }
    username_buf[username_len-1] = '\0';
    
    int password_len = 0;
    while (password_len == 0) {
      printf(1, "Password: ");
      password_len = gets_fd(0, password_buf, sizeof(password_buf) - 1);
    }
    password_buf[password_len-1] = '\0';
    
    if (djb2(username_buf) == true_username_hash &&
        djb2(password_buf) == true_password_hash) {
          printf(1, "Welcome, %s!\n", username_buf);
          exec("sh", argv);
    }
    printf(1, "Incorrect credentials.\n");
  }
}
