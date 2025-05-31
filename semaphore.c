#include "types.h"
#include "defs.h"
#include "semaphore.h"

void sem_init(struct semaphore *sem, int value) {
    cprintf("sem_init: initializing with value %d\n", value);
    sem->value = value;
    initlock(&sem->lock, "semaphore");
    sem->head = 0;
    sem->tail = 0;
    sem->count = 0;
    sem->sleep_channel = (void *)sem; // استفاده از آدرس خود sem به عنوان کانال
}

void sem_acquire(struct semaphore *sem) {
    acquire(&sem->lock);
    cprintf("sem_acquire: value before = %d, pid = %d, head = %d, tail = %d, count = %d\n", sem->value, myproc()->pid, sem->head, sem->tail, sem->count);
    sem->value--;
    if (sem->value < 0) {
        sem->queue[sem->tail] = myproc();
        int old_tail = sem->tail;
        sem->tail = (sem->tail + 1) % 64;
        sem->count++;
        cprintf("sem_acquire: adding pid %d to queue at tail %d, new tail = %d, count = %d\n", myproc()->pid, old_tail, sem->tail, sem->count);
        cprintf("sem_acquire: process %d going to sleep\n", myproc()->pid);
        // استفاده از sleep_channel برای خوابیدن
        sleep(sem->sleep_channel, &sem->lock);
        cprintf("sem_acquire: process %d woke up\n", myproc()->pid);
    }
    cprintf("sem_acquire: value after = %d\n", sem->value);
    release(&sem->lock);
    cprintf("sem_acquire: lock released\n");
}

void sem_release(struct semaphore *sem) {
    acquire(&sem->lock);
    cprintf("sem_release: value before = %d, pid = %d, head = %d, tail = %d, count = %d\n", sem->value, myproc()->pid, sem->head, sem->tail, sem->count);
    sem->value++;
    if (sem->count > 0) {
        struct proc *p = sem->queue[sem->head];
        int old_head = sem->head;
        sem->head = (sem->head + 1) % 64;
        sem->count--;
        cprintf("sem_release: waking up process %d from head %d, new head = %d, count = %d\n", p->pid, old_head, sem->head, sem->count);
        // بیدار کردن با استفاده از sleep_channel
        wakeup(sem->sleep_channel);
    } else {
        cprintf("sem_release: no process to wake up, head = %d, tail = %d, count = %d\n", sem->head, sem->tail, sem->count);
    }
    cprintf("sem_release: value after = %d\n", sem->value);
    release(&sem->lock);
    cprintf("sem_release: lock released\n");
}