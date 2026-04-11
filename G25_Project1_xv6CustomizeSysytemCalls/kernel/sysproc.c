#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
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
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
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
// modified sys_kill function
uint64
sys_kill(void)
{
  int pid, signum;
  argint(0, &pid);
  argint(1, &signum);
  if(signum < 0 || signum >= 32)
    return -1;
  return kkill(pid, signum);
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
//This tells the kernel: When someone asks for the year, give them the number 2026.
uint64
sys_getyear(void)
{
  return 2026;
}
// Register a signal handler for a given signal number
uint64
sys_signal(void)
{
  int signum;
  uint64 handler;
  argint(0, &signum);
  argaddr(1, &handler);
  if(signum < 0 || signum >= 32)
    return -1;
  struct proc *p = myproc();
  p->signal_handlers[signum] = (void (*)(int))handler;
  return 0;
}
// Restore process state after signal handler finishes
uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();
  *p->trapframe = p->signal_trapframe;
  p->in_signal_handler = 0;
  return 0;
}
uint64
sys_sleep(void)
{
  int n;
  uint64 ticks0;
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
