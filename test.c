//A Simple C program
#include "types.h"
#include "stat.h"
#include "user.h"
  
 //passing command line arguments
  
int main(int argc, char *argv[])
{
  printf(1, "%d\n", test());
  for(int i = 1; i < argc; ++i) {
  	printf(1, "%s\n", argv[i]);
  }
  exit();
}

// This code is contributed by sambhav228
