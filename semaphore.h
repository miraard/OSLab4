#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "spinlock.h"
#include "proc.h"

// تعریف ساختار Semaphore
struct semaphore {
    int value;
    struct spinlock lock;
    struct proc *queue[64];
    int head;
    int tail;
    int count;
    void *sleep_channel; // کانال جداگونه برای sleep/wakeup
};

// توابع Semaphore
void sem_init(struct semaphore *sem, int value);
void sem_acquire(struct semaphore *sem);
void sem_release(struct semaphore *sem);

#endif