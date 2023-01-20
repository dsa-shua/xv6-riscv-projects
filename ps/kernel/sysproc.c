#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


// HELPER FUNCTIONS FOR PRINTF without %{spaces}d etc.

//  SPACES
//      Solution really bad. Does a lot of sys calls but works.
//      What it does is it takes a number then counts its digits
//      We define a max space count between each columns
//      for a num with x digits, we count max_space_count - digits
//          as the number of white spaces to be added to fit column.
//      This solution is completely avoidable if the xv6-RISCV's
//          printf function was capable of doing %{spaces}d etc.

// RUNTIME
//      Converts ticks to min:sec.decimal
//      very simple

// isPS()
//      According to the PDF file, we define runtime as the time between
//      the process being created to the time ps() is called.
//      With this in mind, we need to find when ps() was created and
//      all of the runtime values will depend on that. So we find
//      the process named "ps" and retrieve its startTime value which
//      all other processes will depend on.

unsigned MAX_SPACE_COUNT = 8; // %8d format
unsigned digits(int num) {
    unsigned dig = 1;
    while (num / 10){
        dig++;
        num = num / 10;
    }
    return dig;
}

void separate(int num) {
    // PID focus first
    // printf("%d",PID);separate(PID); . . .
    int sep_count = MAX_SPACE_COUNT - digits(num);
    for (int i = 0; i < sep_count; i++) {
        printf(" "); // very bad. does a lot of sys calls
    }
}

void tick2time(unsigned num) {
    // 1 tick == 100ms
    unsigned minutes = num / 600; // 1 min = 60 * 100 ticks
    num = num % 600;
    unsigned seconds = num / 10; // 1 sec = 10 ticks
    num = num % 10;
    
    printf("%d:%d.%d", minutes, seconds, num);
    
    // for alignment purposes
    if (seconds < 10) {
        printf("   "); // 3 spaces
    } else if (seconds >=10) {
        printf("  "); //  2 spaces
    }
}

unsigned isPS(char name[]){
    // check if name is ps character by character.
    // if name is ps, then we return 1
    int similarity = 0;
    for (int i = 0; i < 3; i++) {
        if (i == 0) {
            if (name[i] == 'p'){
                similarity++;
            }
        } else if (i == 1) {
            if (name[i] == 's') {
                similarity++;
            }
        } else if (i == 2) {
            if (name[i] == '\0') {
                similarity++;
            }
        }
    }
    return (similarity == 3) ? 1 : 0;
}


uint64
sys_ps(void)
{
    // EEE3535-01 Operating Systems
    // Assignment 1: Process and System Call

    
    unsigned max_arg_count = 6;
    int args[max_arg_count]; // arg collector list?
    memset(args, 0, max_arg_count * sizeof(int)); // initially set all to 0
    
    // this part collects the inputs from the arg registers using argint();
    for (int i = 0; i < max_arg_count; i++) {
        int *ptrArg = &args[i];
        argint(i,ptrArg);
    }
    
    
    // go through the process list, copy allocproc()'s algorithm
    // store values in here.
    int rPID[NPROC];
    char *rName[NPROC];
    char rState[NPROC];
    unsigned rTicks[NPROC];
    unsigned rNextIndex = 0;
    struct proc *p;
    
    // We go by the rules.. ps_called_tick is the time when ps is created.
    unsigned ps_called_tick = 0;
    for (p = proc; p < &proc[NPROC]; p++) {
        if (p->state != UNUSED) {
            if (isPS(p->name)) {
                ps_called_tick = p->startTime;
            }
        }
    }
    
    // conditions to look for
    int look4Sle = 0; // sleeping
    int look4Run = 0; // runnable
    int look4Exe = 0; // executing
    int look4Zom = 0; // zombie
    int notZeros = 0; // 0 if has no arguments -> ps prints all
    int look4PID[max_arg_count];
    memset(look4PID, 0, max_arg_count * sizeof(int));
    int look4PIDidx = 0; // next index for pid
    

    // print all? check if notZeros == 0.
    for (int i=0; i < max_arg_count; i++) {
        if (args[i] != 0) {
            notZeros = 1;
            goto hasArg;
        }
    }
    
hasArg:
    // specify what we need to collect
    for (int i = 0; i <max_arg_count; i++) {
        if (args[i]==-1) { // sleeping -> 2
            look4Sle = 1;
        } else if (args[i]==-2) {// runnable -> 3
            look4Run = 1;
        } else if (args[i]==-3) { // executing -> 4
            look4Exe = 1;
        } else if (args[i]==-4) { // zombie - >5
            look4Zom = 1;
        } else { // just PID
            if (args[i] != 0) {
                look4PID[look4PIDidx] = args[i];
                look4PIDidx++;
            }
        }
    }
    
    // take valid processes' information and place them to
    //      each of the respective lists.
    if (notZeros == 0) {
        // all processes
        for (p = proc; p < &proc[NPROC]; p++) {
            if (p->state != UNUSED) {
                rPID[rNextIndex] = p->pid;
                rName[rNextIndex] = p->name;
                rTicks[rNextIndex] = ps_called_tick - p->startTime;
                rState[rNextIndex] = p->state;
                rNextIndex++;
            }
        }
    } else {
        // specific processes, take all necessary information and store
        //      it on specified lists above
        for (p = proc; p < &proc[NPROC]; p++) {
            if (p->state != UNUSED) {
                if ((p->state == SLEEPING) && (look4Sle == 1)) { // sleeping
                    rPID[rNextIndex] = p->pid;
                    rName[rNextIndex] = p->name;
                    rTicks[rNextIndex] = ps_called_tick - p->startTime;
                    rState[rNextIndex] = p->state;
                    rNextIndex++;
                    continue;
                } else if ((p->state == RUNNABLE) && (look4Run == 1)) {// runnable
                    rPID[rNextIndex] = p->pid;
                    rName[rNextIndex] = p->name;
                    rTicks[rNextIndex] = ps_called_tick - p->startTime;
                    rState[rNextIndex] = p->state;
                    rNextIndex++;
                    continue;
                } else if ((p->state == RUNNING) && (look4Exe == 1)) {// running
                    rPID[rNextIndex] = p->pid;
                    rName[rNextIndex] = p->name;
                    rTicks[rNextIndex] = ps_called_tick - p->startTime;
                    rState[rNextIndex] = p->state;
                    rNextIndex++;
                    continue;
                } else if ((p->state == ZOMBIE) && (look4Zom == 1)) {// zombie
                    rPID[rNextIndex] = p->pid;
                    rName[rNextIndex] = p->name;
                    rTicks[rNextIndex] = ps_called_tick - p->startTime;
                    rState[rNextIndex] = p->state;
                    rNextIndex++;
                    continue;
                } else { // just pid
                    for (int i = 0; i < max_arg_count; i++) {
                        if (p->pid == look4PID[i]) {
                            rPID[rNextIndex] = p->pid;
                            rName[rNextIndex] = p->name;
                            rTicks[rNextIndex] = ps_called_tick - p->startTime;
                            rState[rNextIndex] = p->state;
                            rNextIndex++;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    // print results;
    
    // Printing using printf() specified by xv6-RISCV without
    //      modifying code to accept %{spaces}d etc...
    // We use the functions defined above to help print the results nicely.
    
    printf("PID     State   Runtime Name\n"); // header. very important
    for (int i = 0; i < rNextIndex; i++){
        if (rState[i] == SLEEPING) {
            printf("%d", rPID[i]); separate(rPID[i]);
            printf("S"); separate(0);
            tick2time(rTicks[i]);
            printf("%s\n",rName[i]);
        } else if (rState[i] == RUNNABLE) {
            printf("%d", rPID[i]); separate(rPID[i]);
            printf("R"); separate(0);
            tick2time(rTicks[i]);
            printf("%s\n",rName[i]);
        } else if (rState[i] == RUNNING) {
            printf("%d", rPID[i]); separate(rPID[i]);
            printf("X"); separate(0);
            tick2time(rTicks[i]);
            printf("%s\n",rName[i]);
        } else if (rState[i] == ZOMBIE) {
            printf("%d", rPID[i]); separate(rPID[i]);
            printf("Z"); separate(0);
            tick2time(rTicks[i]);
            printf("%s\n",rName[i]);
        }
    }
  return 0;
}
