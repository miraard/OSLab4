#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "spinlock.h"
#include "proc.h"

// تعریف ساختار Semaphore
struct semaphore {
  int value;           // مقدار اولیه یا فعلی Semaphore
  struct spinlock lock;   // قفل برای محافظت از دسترسی هم‌زمان
  struct proc *queue[64]; // صف فرآیندهایی که منتظر Semaphore هستن
  int head, tail;      // اندیس‌های ابتدا و انتهای صف
};

// توابع Semaphore
void sem_init(struct semaphore *sem, int value);
void sem_acquire(struct semaphore *sem);
void sem_release(struct semaphore *sem);

#endif