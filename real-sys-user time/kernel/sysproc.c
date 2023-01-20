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

// I called this for scanning through the proceses.
// See sys_gettime() comments for more.
extern struct proc proc[NPROC];


// Helper function that converts ticks to ( )m:( ).( )s
void dispTime(int time, unsigned what) {
  // 0 : real time, 1: user, 2: sys
  /* VERY IMPORTANT */
  
  if (what == 0){
    printf("real: ");
  } else if (what == 1) {
    printf("user: ");
  } else if (what == 2) {
    printf("sys:  ");
  } else {
//    printf("need to specify what type of time to print\n"); exit(1);
  }
  
  int minutes = time / 6000;
  time = time % 6000;
  printf("%dm", minutes);
  
  int seconds = time / 100;
  time = time % 100;
  printf("%d.", seconds);
  
  /*
   The following fixes the seconds part.
   Because xv6-RISCV has no support for
   formatting decimal points etc....
   */
  
  if (time >= 10) {
    printf("%ds\n",time);
  } else {
    printf("0%ds\n",time);
  }
}

uint64
sys_gettime(void){

  // for some reason I could not go through all the active processes
  // using allocproc()'s algorithm which I used for assingment  1.
  
  // the error was: p = proc "not defined" or something.
  
  // Hence, I decided to "import" (idk what it is called in the C programming world)
  // the struc proc proc[NPROC] thing and seearch through the first address.
  
  
  struct proc *p = &proc[0];
  
  unsigned idx = 0;
  while (p < &proc[NPROC]){
    if (p->state == RUNNING) {
      // well, the only running program is the one who called gettime()...
      
      /* WORKING CODE FOR REAL TIME*/
      
//      int realTime = ticks - p->startTime;
      
      /* END */
      
      int sysTime = p->sysTimeChild;
      int userTime = p->userTimeChild;
      
      /* TESTING FOR REALTIME */
      int realTime = p->realTimeChild;
      
      /*
       The testing code works I guess.
       I'll keep it.
       
       */
      
      // USE HELPER FUNCTIONS TO PRINT TIME
      
      dispTime(realTime, 0);
      dispTime(userTime,1);
      dispTime(sysTime,2);
    }
    idx++;
    p++;
    if (idx > 90){
      // just incase the loop goes wild for some reason.
      break;
    }
  }
  return 0;
}
