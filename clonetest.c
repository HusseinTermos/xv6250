#include "types.h"
#include "stat.h"
#include "user.h"
#include "mmu.h" // For PGSIZE

// #define PGSIZE 0x1000

int var_in_shared_mem = 0;

void factorial(void* arg) {
    printf(1, "Child: My pid is %d\n", getpid());
    int* arg_as_int_p = (int*) arg;
    int fact = 1;
    for (int i = 1; i <= *arg_as_int_p; i++) fact *= i;
    printf(1, "Child received arg: %p storing int value %d.\n", arg_as_int_p, *arg_as_int_p);
    printf(1, "Child outputs %d.\n");
    exit();
}

int main() {
    int pid = getpid();
    printf(1, "Parent pid is: %d\n", pid);
    char* stack_pointer = sbrk(PGSIZE);
    int local_arg = 5;
    int child_pid = clone(factorial, (void*) &local_arg, stack_pointer);
    printf(1, "Parent: My child has pid %d\n", child_pid);
    sleep(10);
    printf(1, "Safe for parent to assume that child terminated.\n");
    // wait();
    exit();
}