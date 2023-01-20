#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// A xv6-riscv syscall can take up to six arguments.
#define max_args 6

// Print a help message.
void print_help(int argc, char **argv) {
  fprintf(2, "%s <options: pid or S/R/X/Z>%s\n",
             argv[0], argc > 7 ? ": too many args" : "");
}

int main(int argc, char **argv) {
    
    /*  Notes:
     
     We want to convert str inputs to int.
     We set the encoding for the process states as follows:
     S -> -1
     R -> -2
     X -> -3
     Z -> -4
     
     We first check them as they are probably the most common inputs.
     
     For the pid inputs, it is just an int type.
     However, we also check if the int input is not erronous.
     
     
     
     atoi() limitations:
     
     atoi() will convert str to int but if input to atoi is like 1a2,
     then atoi will return 1, which is questionable and 1a2 is invalid.
     
     MY SOLUTION:
     scan each character on the string and check for something invalid.
     then we call the print_help() function then exit(1).
     
     Yes, my solution is not the best solution as it is slow.
     Please dont mind it as long as the program works properly :)
     
     
     
     printf() limitations:
     
     printf() in xv6-RISCV is also limited. I defined some helper functions
     in sysproc.c file to help deal with the alignments of the columns but
     the solution is also not the best as it does a lot of printf() to
     add spaces necessary to keep alignment.
     
     The alignment for time is set to only be correct upto 9:59.9
     If somehow time goes over 10 minutes, then the alignment
     would not be correct. This might not be a problem as we can safely say
     for now, we do not run processes for more than 10 minutes.
     
     
     
     About case sensitivity:
     
     This program does not accept lower cased inputs. Although not difficult to
     implement, I decided not to for simplicity.
     
     
     */
    
    // Print a help message.
    if(argc > 7) { print_help(argc, argv); exit(1); }
    
    
    // Argument vector for ps() below
    int args[max_args];
    memset(args, 0, max_args * sizeof(int)); // initially sets the values to 0
    
    // encode string inputs to pids
    // input args to some int
    
    for (int i = 1; i < argc; i++){
        // encode states, keeping it simple
        if (*argv[i] == 'S') {
            args[i-1] = -1;
        } else if (*argv[i] == 'R') {
            args[i-1] = -2;
        } else if (*argv[i] == 'X') {
            args[i-1] = -3;
        } else if (*argv[i] == 'Z') {
            args[i-1] = -4;
        } else { // Has either valid or invalid inputs
            
            int j = 0;
            int found = 0; // found an invalid input
            
            // check until we found end of string or found an error
            while ((argv[i][j] != '\0') && (found == 0)){
                if ((atoi(&argv[i][j]) == 0) && (argv[i][j] != '0')) {
                    // if atoi gave a zero for a char but it is not 0,
                    //      then it is a letter in the middle -> invalid.
                    print_help(argc, argv);
                    found = 1;}
                j++;}
            if (!found) {
                args[i-1] = atoi(argv[i]);
            } else if (found) {
                exit(1);
            }
        }
    }
    
// explicit input of 0 means we are looking for a process with pid = 0;
    // but that will never exist hence we would look for a process with pid = -9
    // which will never exist hence skips it.
    // this solves the problem when $ ps 0
    // this will print nothing.

    for (int i = 0; i <max_args; i++){
        if (argc != 1) {
            if (args[i] == 0) {
                args[i] = -9;
            }
        }
    }

    
  // Call the ps() syscall.
    int ret = ps(args[0], args[1], args[2], args[3], args[4], args[5]);
    if(ret) { fprintf(2, "ps failed\n"); exit(1); }
    
    exit(0);
}
