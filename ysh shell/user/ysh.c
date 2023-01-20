#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define buf_size    128     // Max length of user input
#define max_args    16      // Max number of arguments

int runcmd(char *cmd);      // Run a command.

// Read a shell input.
char* readcmd(char *buf) {
    // Read an input from stdin.
    fprintf(1, "ysh $ ");
    memset(buf, 0, buf_size);
    char *cmd = gets(buf, buf_size);
  
    // Chop off the trailing '\n'.
    if(cmd) { cmd[strlen(cmd)-1] = 0; }
  
    return cmd;
}

int main(int argc, char **argv) {
    int fd = 0;
    char *cmd = 0;
    char buf[buf_size];
  
    // Ensure three file descriptors are open.
    while((fd = open("console", O_RDWR)) >= 0) {
        if(fd >= 3) { close(fd); break; }
    }
  
    fprintf(1, "starting ysh\n");
  
    // Read and run input commands.
    while((cmd = readcmd(buf)) && runcmd(cmd)) ;
  
    fprintf(1, "closing ysh\n");
    exit(0);
}

// Run a command.
int runcmd(char *cmd) {
    if(!*cmd) { return 1; }                     // Empty command

    // Skip leading white space(s).
    while(*cmd == ' ') { cmd++; }
    // Remove trailing white space(s).
    for(char *c = cmd+strlen(cmd)-1; *c == ' '; c--) { *c = 0; }

    if(!strcmp(cmd, "exit")) { return 0; }      // exit command
    else if(!strncmp(cmd, "cd ", 3)) {          // cd command
        if(chdir(cmd+3) < 0) { fprintf(2, "Cannot cd %s\n", cmd+3); }
    }
    else {
      
//      printf("cmd: %s\n",cmd);
      // EEE3535-01 Operating Systems
      // Assignment 3: Shell
      
      /*
       
       IMPORTANT MECHANISM:
       
       Since the runcmd() function returns 1, it does not kill the
       subsequent fork() children processes but instead keeps it
       running in the background. Hence, we kill() the children
       using getpid() and kill().
       
       This ensures that there is only 1 ysh shell running when the
       child processes are running.
       
       
       */
      
      
      unsigned background = 0; // &
      unsigned multi = 0; // ;
      unsigned s = 0; // location of first ;
      unsigned piped = 0; // |
      unsigned p = 0; // location of first |
      
      if (cmd[strlen(cmd)-1] == '&') {
//        printf("background process\n");
        background++;
      }
      
      if (background) {
        int rc = fork();
        if (rc < 0) {printf("fork failed!\n");}
        else if (rc > 0) {
//          printf("background parent process\n");
        } else {
//          printf("background child process\n");
          cmd[strlen(cmd)-1] = '\0';
          runcmd(cmd);
          unsigned pid = getpid(); kill(pid);
        }
      }

      else {
        
        // check if multi command
        for (unsigned i = 0; i < strlen(cmd); i++){
          if (cmd[i] == ';'){
            s = i;
            multi++;
            break;
          }
        }
        
        // check if piped
        
        for (unsigned i = 0; i < strlen(cmd); i++) {
          if (cmd[i] == '|'){
            p = i;
            piped++;
            break;
          }
        }
        
        if (multi) {
          cmd[s] = '\0';
//          printf("is multi\n");
//          printf("multi cmd child: %s\n", cmd);
//          printf("multi cmd parent: %s\n", cmd+s+1);
          int multiRC = fork();
          if (multiRC < 0) {fprintf(2,"fork() failed!\n"); exit(1);}
          else if (multiRC > 0) {
            wait(0);
//            printf("multi parent process\n");
            runcmd(cmd+s+1);
          } else {
//            printf("multi child process\n");
            runcmd(cmd);
            unsigned pid = getpid(); kill(pid);
          }
        } // if (multi)
        
        else if (!multi && piped) {
          // is piped
//          printf("is piped\n");
          cmd[p] = '\0'; // set pipe char to NULL
          
          int pipeRC = fork();
          
          if (pipeRC < 0) {fprintf(2,"fork() failed!\n"); exit(1);}
          else if (pipeRC > 0) {
            wait(0);
//            printf("parent of piped\n");
          } else {
//            printf("child of piped\n");
            
            
            int _p[2]; // pipe
            if (pipe(_p) != 0) {fprintf(2,"pipe() failed!\n"); exit(1);}
            else { // pipe is working
              //
//              printf("left cmd: %s\n", cmd);
//              printf("right cmd: %s\n", cmd+p+1);
              //
              int subPipe = fork();
              if (subPipe < 0) {fprintf(2,"fork() failed!\n"); exit(1);}
              else if (subPipe > 0) {
                // read end of pipe
//                wait(0);
//                printf("read end of pipe\n");
                close(0);
                dup(_p[0]);
                close(_p[0]); close(_p[1]);
                runcmd(cmd+p+1);
//                printf("killing read end of pipe\n");
                unsigned readpid = getpid(); kill(readpid);
              } // parent subpipe fork
              else {
                // write end of pipe
                
//                printf("write end of pipe\n");
                close(1);
                dup(_p[1]);
                close(_p[0]); close(_p[1]);
                runcmd(cmd);
//                printf("killing write end of pipe\n");
                unsigned writepid = getpid(); kill(writepid);
              }
              wait(0); wait(0);
              close(_p[0]); close(_p[1]);
            }
          }
        }
        else {
            // simple case
          int rc = fork();
          if (rc < 0) {fprintf(2, "fork() failed!\n"); exit(1);}
          else if (rc > 0) {wait(0);
          }
          else {
            
            /*
             
             To run the simplest case, we separate the cmd by whitespace.
             The separated "words" are placed into an array
             
             */
            unsigned whiteSpaces = 0;
            unsigned spaceLoc[128];
            unsigned len = strlen(cmd);
            
            for (unsigned i = 0; i < len; i++){
              if (cmd[i] == ' '){
                spaceLoc[whiteSpaces] = i;
                whiteSpaces++;
                cmd[i] = '\0';
              }
            }
            char *argv[whiteSpaces+1];
            argv[0] = cmd;
            for (unsigned i = 0; i < whiteSpaces; i++){
              argv[i+1] = cmd+spaceLoc[i]+1;
            }
            argv[whiteSpaces+1] = '\0';
            exec(argv[0], argv);
            printf("exec failed! <%s>\n", argv[0]);
            unsigned pid = getpid(); kill(pid);
          }
        }
      }
    }
  return 1;
}
