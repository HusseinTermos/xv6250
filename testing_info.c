// infodemo.c — demo of set_info / info_wait on xv6 (x86)
// Build: add _infodemo to UPROGS in Makefile, then `make qemu`

#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int pid = fork();
  if(pid < 0){
    printf(1, "fork failed\n");
    exit();
  }

  if(pid == 0){
    // --- CHILD ---
    // Compose any bytes you like (string, TLV, struct-as-bytes, etc.)
    char payload[128];
    int n = 0;

    // simple example: a tiny message with an integer
    n += snprintf(payload + n, sizeof(payload) - n, "status=%d;", 7);
    n += snprintf(payload + n, sizeof(payload) - n, "msg=%s", "bye parent");

    if(n < 0) n = 0;
    if(n > (int)sizeof(payload)) n = sizeof(payload);

    // Stash into kernel (truncation is handled in the syscall if needed)
    int stored = set_info(payload, n);
    if(stored < 0)
      printf(1, "child: set_info failed\n");

    exit(); // child done
  }

  // --- PARENT ---
  char buf[128];
  int got = 0;

  int cpid = info_wait(buf, sizeof(buf), &got);
  if(cpid < 0){
    printf(1, "parent: info_wait failed\n");
    exit();
  }

  // Ensure printable (null-terminate for printf if it's textual)
  if(got < 0) got = 0;
  if(got >= (int)sizeof(buf)) got = sizeof(buf) - 1;
  buf[got] = 0;

  printf(1, "parent: reaped child %d, got %d byte(s): \"%s\"\n", cpid, got, buf);
  exit();
}
