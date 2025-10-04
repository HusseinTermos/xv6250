#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int pid = fork();

  if (pid < 0) {
    printf(1, "fork failed\n");
  }

  if (pid == 0) { // Child
    char info[] = "We can store anything here; not necessarily characters.";
    int info_len = sizeof(info);
    set_info(info, info_len);
    exit();
  }
  else { // Parent
    char info_buf[128];
    int info_len;

    int cpid = info_wait(info_buf, 128, &info_len);
    info_buf[info_len] = '\0';
    printf(1, "Length of Received Message: %d\nReceived Message: %s\n", info_len, info_buf);
    exit();
  }
}
