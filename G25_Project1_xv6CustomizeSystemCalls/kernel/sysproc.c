#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
extern struct semaphore global_sem;
extern struct proc proc[NPROC];
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
// ================= IPC SYSTEM CALLS =================


uint64
sys_send(void)
{
  int pid;
  char msg[64];

  argint(0, &pid);
  argstr(1, msg, sizeof(msg));

  struct proc *p;

  for(p = proc; p < &proc[NPROC]; p++){
    if(p->pid == pid){
      safestrcpy(p->message, msg, sizeof(p->message));
      p->has_message = 1;
      return 0;
    }
  }

  return -1;
}


uint64
sys_recv(void)
{
  struct proc *p;
  int found = 0;

  for(p = proc; p < &proc[NPROC]; p++){
    if(p->has_message){
      printf("Received from PID %d: %s\n", p->pid, p->message);
      p->has_message = 0;
      found = 1;
    }
  }

  if(!found){
    printf("No message\n");
    return -1;
  }

  return 0;
}



uint64
sys_sem_init(void) {
  int val;
  argint(0, &val);

  acquire(&global_sem.lock);
  global_sem.value = val;
  release(&global_sem.lock);

  return 0;
}


uint64
sys_sem_wait(void) {
  acquire(&global_sem.lock);

  while(global_sem.value <= 0){
    sleep(&global_sem, &global_sem.lock);
  }

  global_sem.value--;

  release(&global_sem.lock);
  return 0;
}


uint64
sys_sem_post(void) {
  acquire(&global_sem.lock);

  global_sem.value++;
  wakeup(&global_sem);

  release(&global_sem.lock);
  return 0;
}

// Riyan - Lock System Calls

uint64
sys_lock_create(void) {
  for(int i = 0; i < MAX_LOCKS; i++) {
    if(lock_used[i] == 0) {
      initlock(&userlocks[i], "ulock");
      lock_used[i] = 1;
      return i;
    }
  }
  return -1;
}

uint64
sys_lock_acquire(void) {
  int id;
  argint(0, &id);

  if(id < 0 || id >= MAX_LOCKS || lock_used[id] == 0)
    return -1;

  acquire(&userlocks[id]);
  return 0;
}

uint64
sys_lock_release(void) {
  int id;
  argint(0, &id);

  if(id < 0 || id >= MAX_LOCKS || lock_used[id] == 0)
    return -1;

  release(&userlocks[id]);
  return 0;
}
