#include "kernel/types.h"
#include "user/user.h"


int main(int argc, char **argv) {
  // Syscall for MLFQ stats: pass 1 for reset, otherwise 0 to print stats.
  mlfq((argc > 1) && !strcmp(argv[1], "reset"));
  exit(0);
}
