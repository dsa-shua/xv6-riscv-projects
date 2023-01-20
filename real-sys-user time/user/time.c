#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// just in case that the user had entered "time on the shell"
// we just print an error message

int main(void){
  printf("Usage time <programs>\n");
  return 0;
}
