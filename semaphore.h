#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "types.h"
#include "spinlock.h"

struct semaphore {
    int value;
    struct spinlock lock;
    struct proc *queue[64];
    int head;
    int tail;
    int count;
    struct proc *waiters[64];
    int waiter_count;
    struct spinlock sleeplock;
};

int sem_init(int id, int value);
int sem_acquire(int id);
int sem_release(int id);

#endif