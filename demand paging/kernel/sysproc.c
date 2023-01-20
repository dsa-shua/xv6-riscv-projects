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
  return addr; //returns the addr of the new requested block
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


extern uint64 ZEROFRAME;


uint64
sys_getPA(void){
   //DEBUGGING ONLY
  printf("sys_getPA called\n");
//
//  printf("PTE_V: %d\n", (int)PTE_V);
//  printf("PTE_R: %d\n", (int)PTE_R);
//  printf("PTE_W: %d\n", (int)PTE_W);
//  printf("PTE_X: %d\n", (int)PTE_X);
//  printf("PTE_U: %d\n", (int)PTE_U);
  
//#ifdef DEBUG
//  printf("ZEROFRAME: %p\n", (void*)ZEROFRAME);
//#endif
  
  
  /*
   Based on the test, zero frame is this guy:
   
   0x0000000087fff000
   
   which is the very first physical address that kaloc() gives.
   
   */
  
  // retrive VA
//  int virtual_address=0;
//
//  argint(0, &virtual_address);
//
//  // check heap size
//  struct proc* myproc();
//  printf("heap size: %d\n",myproc()->sz);
//
//  // get PA
//  if (virtual_address !=0) {
//    uint64 ret_va = 0x0 | virtual_address; // extend to 64 bits
//    printf("retrieved va: %p\n", (void*)ret_va);
//
//    uint64 physical_address = walkaddr(myproc()->pagetable, ret_va);
//    printf("retrieved pa: %p\n", (void*)physical_address);
//
//    return physical_address;
//  }
//
//  // check kalloc() mem..
//  if (virtual_address == -1) {
//    printf("");
//  }
  
  return 0;
}



