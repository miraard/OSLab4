#include "types.h"
#include "defs.h"
#include "semaphore.h"

void sem_init(struct semaphore *sem, int value) {
    sem->value = value;
    initlock(&sem->lock, "semaphore");
    sem->head = 0;
    sem->tail = 0;
}

void sem_acquire(struct semaphore *sem) {
    acquire(&sem->lock);
    sem->value--;
    if (sem->value < 0) {
        sem->queue[sem->tail] = myproc();
        sem->tail = (sem->tail + 1) % 64;
        sleep(myproc(), &sem->lock);
    }
    release(&sem->lock);
}

void sem_release(struct semaphore *sem) {
    acquire(&sem->lock);
    sem->value++;
    if (sem->value <= 0 && sem->head != sem->tail) {
        struct proc *p = sem->queue[sem->head];
        sem->head = (sem->head + 1) % 64;
        wakeup(p);
    }
    release(&sem->lock);
}