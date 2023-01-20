#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



// DUMMY FORK TESTING PROGRAM
// I USED THIS TO CHECK WHY FORKTEST WAS NOT WORKING PROPERLY.

// NOTHING SPECIAL HERE
int main(void){
  
  int rc = fork();
  
  if (rc < 0) {
    fprintf(2, "fork failed!\n"); exit(1);
  } else if (rc > 0){ // parent process
    wait(0);
//    printf("parent process\n");
    
    int rc1 = fork();
    if (rc1 < 0) {
      fprintf(2,"2nd fork failed!\n"); exit(1);
    } else if (rc > 0) {
//      printf("second child process");
      
      unsigned idx = 0;
      while (idx < 1000) {
        idx++;
        printf(""); // force a lot of sys calls
      }
    } else {
      wait(0);
      printf("the same parent?\n");
      
      unsigned idx = 0;
      while (idx < 1000000) {
        idx++;
      }
    }
  } else {// child process
//    printf("first child process\n");
    unsigned idx = 0;
    
    while (idx < 20000000) {
      while (idx< 100000) {
        idx++; printf("");
      }
      idx++;
    }
  }
  
  return 0;
}
