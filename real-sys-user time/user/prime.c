#include "kernel/types.h"
#include "user/user.h"

// Find the Nth prime number.
unsigned get_nth_prime(unsigned n) {
  if(n == 1) { return 2; }
  unsigned i = 2, j = 0, k = 0;
  for(j = 3; i < n; ) {
    for(j += 2, k = 3; k < j; k += 2) { if(j % k == 0) { break; } }
    if(j == k) { i++; }
  }
  return j;
}

int main(int argc, char **argv) {
  if(argc != 2) {
      printf("%s <Nth prime # to find>\n", argv[0]);
      exit(0);
  }

  unsigned N = atoi(argv[1]);
  char ord[3];
  switch(N % 10) {
    case 1:            { strcpy(ord, "st"); break; }
    case 2:            { strcpy(ord, "nd"); break; }
    case 3:            { strcpy(ord, "rd"); break; }
    default:           { strcpy(ord, "th"); break; }
  } if((N/10)%10 == 1) { strcpy(ord, "th");        }

  printf("%d%s prime number is %d\n", N, ord, get_nth_prime(N));

  exit(0);
}
