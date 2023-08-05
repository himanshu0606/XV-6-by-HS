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

// below waitx taken from tut 
uint64
sys_waitx(void)
{
  uint64 addr, addr1, addr2;
  uint wtime, rtime;
  argaddr(0, &addr);
  argaddr(1, &addr1); // user virtual memory
  argaddr(2, &addr2);
  int ret = waitx(addr, &wtime, &rtime);
  struct proc* p = myproc();
  if (copyout(p->pagetable, addr1,(char*)&wtime, sizeof(int)) < 0)
    return -1;
  if (copyout(p->pagetable, addr2,(char*)&rtime, sizeof(int)) < 0)
    return -1;
  return ret;
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

uint64
sys_settickets(void)
{

    int arg;
    argint(0, &arg);

    return settickets(arg);
}

uint64
sys_set_priority(void){

    int pid;
    int prioritySet;

    argint(0, &prioritySet);
    argint(1, &pid);


    return set_priority(prioritySet, pid);
}

uint64 sys_trace()
{
  int HA_mask;

  // is the argument passed by user is invalid, we will return -1
  // if(HA_mask<0)
  //   return -1;

  
  argint(0,&HA_mask);
  myproc()->HA_mask = HA_mask;
  return 0;
}

// HA:- functions for sigalarm and sigreturn

uint64 sys_sigalarm(void)
{
  int tick_cnt;
  argint(0,&tick_cnt);
  uint64 handler;
  argaddr(1,&handler);
  myproc()->alarm_on = 1;
  myproc()->ticks = tick_cnt;
  myproc()->current_ticks = 0;
  myproc()->handler = handler;
  return 0;
}

// HA:- making a function to get required register values in our copy trapframe and clearing the original trapframe

void restore_value()
{
  struct proc *p = myproc();
  p->copy->kernel_satp = p->trapframe->kernel_satp;
  p->copy->kernel_sp = p->trapframe->kernel_sp;
  p->copy->kernel_trap = p->trapframe->kernel_trap;
  p->copy->kernel_hartid = p->trapframe->kernel_hartid;
  *(p->trapframe) = *(p->copy);
}

uint64 sys_sigreturn(void)
{
  restore_value();
  myproc()->alarm_on = 1;
  return myproc()->trapframe->a0;
}



/*
 *     for(int i = 0; i < NPROC; i++){
        acquire(proc[i]->lock);
        if(proc[i]->pid == pid){
            currentProc = proc[i];
             int temp;
             temp = currentProc->priority;
             int temparg;
             argint(0, &temparg);
             currentProc->priority = temparg;
             release(proc[i]->lock);
            if(temp > temparg ){
               yield();
            }
             return temp;
        }
        release(proc[i]->lock);
    }

 *
 */