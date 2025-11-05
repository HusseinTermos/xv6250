//A Simple C program
#include "types.h"
#include "stat.h"
#include "user.h"
  
 //passing command line arguments
  
int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf(2, "Usage: %s file_name1 file_name2\n", argv[0]);
    exit();
  }
  
}
