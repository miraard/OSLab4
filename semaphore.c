#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "semaphore.h"

#define MAX_SEMAPHORES 10
struct semaphore semaphores[MAX_SEMAPHORES];
int semaphore_count = 0;

int sem_init(int id, int value) {
if (id < 0 || id >= MAX_SEMAPHORES) {
cprintf("sem_init: invalid id %d\n", id);
return -1;
}
if (semaphores[id].lock.locked) {
cprintf("sem_init: semaphore %d already initialized\n", id);
return -1;
}
cprintf("sem_init: initializing with value %d for id %d\n", value, id);
semaphores[id].value = value;
initlock(&semaphores[id].lock, "semaphore");
initlock(&semaphores[id].sleeplock, "sleeplock");
semaphores[id].head = 0;
semaphores[id].tail = 0;
semaphores[id].count = 0;
semaphores[id].waiter_count = 0;
for (int i = 0; i < 64; i++) {
semaphores[id].waiters[i] = 0;
}
semaphore_count++;
return 0;
}

int sem_acquire(int id) {
if (id < 0 || id >= MAX_SEMAPHORES) {
cprintf("sem_acquire: invalid id %d\n", id);
return -1;
}
struct semaphore *sem = &semaphores[id];
acquire(&sem->lock);
cprintf("sem_acquire: value before = %d, pid = %d, head = %d, tail = %d, count = %d\n", sem->value, myproc()->pid, sem->head, sem->tail, sem->count);
cprintf("sem_acquire: waiter_count before = %d\n", sem->waiter_count);
int initial_value = sem->value; // ذخیره مقدار اولیه برای دیباگ
sem->value--;
cprintf("sem_acquire: value after decrement = %d (initial = %d)\n", sem->value, initial_value);
if (sem->value < 0) {
sem->queue[sem->tail] = myproc();
int old_tail = sem->tail;
sem->tail = (sem->tail + 1) % 64;
sem->count++;
cprintf("sem_acquire: adding pid %d to queue at tail %d, new tail = %d, count = %d\n", myproc()->pid, old_tail, sem->tail, sem->count);

int waiter_idx = 0;
for (int i = 0; i < 64; i++) {
if (sem->waiters[i] == 0) {
waiter_idx = i;
break;
}
}
sem->waiters[waiter_idx] = myproc();
sem->waiter_count++;
cprintf("sem_acquire: adding pid %d to waiters at idx %d, waiter_count = %d\n", myproc()->pid, waiter_idx, sem->waiter_count);

cprintf("sem_acquire: process %d going to sleep\n", myproc()->pid);
cprintf("sem_acquire: before sleep, count = %d, waiter_count = %d\n", sem->count, sem->waiter_count);
acquire(&sem->sleeplock);
release(&sem->lock);
sleep(sem, &sem->sleeplock);
release(&sem->sleeplock);
acquire(&sem->lock);
cprintf("sem_acquire: process %d woke up, count = %d, waiter_count = %d\n", myproc()->pid, sem->count, sem->waiter_count);

for (int i = 0; i < 64; i++) {
if (sem->waiters[i] == myproc()) {
sem->waiters[i] = 0;
sem->waiter_count--;
cprintf("sem_acquire: removed pid %d from waiters at idx %d, waiter_count = %d\n", myproc()->pid, i, sem->waiter_count);
break;
}
}
} else {
cprintf("sem_acquire: no sleep needed, value = %d\n", sem->value);
}
cprintf("sem_acquire: value after = %d\n", sem->value);
release(&sem->lock);
cprintf("sem_acquire: lock released\n");
return 0;
}

int sem_release(int id) {
if (id < 0 || id >= MAX_SEMAPHORES) {
cprintf("sem_release: invalid id %d\n", id);
return -1;
}
struct semaphore *sem = &semaphores[id];
acquire(&sem->lock);
cprintf("sem_release: value before = %d, pid = %d, head = %d, tail = %d, count = %d\n", sem->value, myproc()->pid, sem->head, sem->tail, sem->count);
cprintf("sem_release: waiter_count before = %d\n", sem->waiter_count);
int initial_value = sem->value; // ذخیره مقدار اولیه برای دیباگ
sem->value++;
cprintf("sem_release: value after increment = %d (initial = %d)\n", sem->value, initial_value);
if (sem->count > 0 || sem->waiter_count > 0) {
struct proc *p = (sem->head < 64) ? sem->queue[sem->head] : 0;
if (p != 0) {
int old_head = sem->head;
sem->head = (sem->head + 1) % 64;
sem->count--;
cprintf("sem_release: waking up process %d from head %d, new head = %d, count = %d\n", p->pid, old_head, sem->head, sem->count);
} else {
cprintf("sem_release: queue empty, but waiter_count = %d\n", sem->waiter_count);
}
cprintf("sem_release: before wakeup, waiter_count = %d\n", sem->waiter_count);
release(&sem->lock);
acquire(&sem->sleeplock);
cprintf("sem_release: waking up processes, waiter_count = %d\n", sem->waiter_count);
wakeup(sem);
release(&sem->sleeplock);
acquire(&sem->lock);
cprintf("sem_release: after wakeup, waiter_count = %d\n", sem->waiter_count);
} else {
cprintf("sem_release: no process to wake up, head = %d, tail = %d, count = %d\n", sem->head, sem->tail, sem->count);
}
cprintf("sem_release: value after = %d\n", sem->value);
release(&sem->lock);
cprintf("sem_release: lock released\n");
return 0;
}   