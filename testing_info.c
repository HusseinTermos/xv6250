// infodemo.c — demo for set_info / info_wait in xv6 (x86)

// Build: add _infodemo to UPROGS in the Makefile, then `make qemu`
// UPROGS=… _infodemo\

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
    char payload[] = "Hello from child!";
    int len = sizeof(payload); // includes terminating '\0'
    int x;
    if(((x = set_info(payload, len)) < 0))
      printf(1, "child: set_info failed\n");
    exit();
  }

  // --- PARENT ---
  char buf[128];
  int got = 0;

  int cpid = info_wait(buf, sizeof(buf), &got);
  if(cpid < 0){
    printf(1, "parent: info_wait failed\n");
    exit();
  }

  // Ensure string termination
  if(got <= 0) got = 0;
  if(got >= (int)sizeof(buf)) got = sizeof(buf) - 1;
  buf[got] = 0;

  printf(1, "parent: reaped child %d, got %d bytes: \"%s\"\n", cpid, got, buf);
  exit();
}
