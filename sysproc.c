#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "semaphore.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_sem_init(void)
{
int id, value;
if (argint(0, &id) < 0 || argint(1, &value) < 0)
return -1;
return sem_init(id, value);
}

int
sys_sem_acquire(void)
{
int id;
if (argint(0, &id) < 0)
return -1;
return sem_acquire(id);
}

int
sys_sem_release(void)
{
int id;
if (argint(0, &id) < 0)
return -1;
return sem_release(id);
}


// اضافه کردن سیستم‌کال‌های جدید برای Reader-Writer
int
sys_rw_init(void)
{
rw_init();
return 0;
}

int
sys_reader(void)
{
reader();
return 0;
}

int
sys_writer(void)
{
writer();
return 0;
}

int
sys_barber_init(void)
{
return barber_init();
}

int
sys_barber(void)
{
return barber();
}

int
sys_customer(void)
{
return customer();
}
