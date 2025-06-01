#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "semaphore.h"

#define RW_SEM_ID 1
#define MUTEX_SEM_ID 2

int read_count = 0;
struct spinlock rw_lock;

int rw_init(void) {
initlock(&rw_lock, "rw_lock");
sem_init(RW_SEM_ID, 1);    // فقط یه Writer یا گروهی از Readerها
sem_init(MUTEX_SEM_ID, 1); // برای همگام‌سازی read_count
cprintf("rw_init: initialized with rw_sem_id=%d, mutex_sem_id=%d\n", RW_SEM_ID, MUTEX_SEM_ID);
return 0;
}

void delay(int ticks) {
for (int i = 0; i < ticks * 100; i++) {
yield(); // کنترل رو به فرآیندهای دیگه بده
}
}

int reader(void) {
sem_acquire(MUTEX_SEM_ID); // قفل برای افزایش read_count
read_count++;
if (read_count == 1) {
sem_acquire(RW_SEM_ID); // اگه اولین Reader باشه، قفل رو بگیر
}
sem_release(MUTEX_SEM_ID); // آزاد کردن قفل mutex

// بخش بحرانی: خواندن داده
cprintf("Reader %d: reading, read_count=%d\n", myproc()->pid, read_count);
delay(10); // شبیه‌سازی زمان خواندن

sem_acquire(MUTEX_SEM_ID); // قفل برای کاهش read_count
read_count--;
if (read_count == 0) {
sem_release(RW_SEM_ID); // اگه آخرین Reader باشه، قفل رو آزاد کن
}
sem_release(MUTEX_SEM_ID); // آزاد کردن قفل mutex
cprintf("Reader %d: finished reading\n", myproc()->pid);
return 0;
}

int writer(void) {
sem_acquire(RW_SEM_ID); // قفل انحصاری برای Writer

// بخش بحرانی: نوشتن داده
cprintf("Writer %d: writing\n", myproc()->pid);
delay(15); // شبیه‌سازی زمان نوشتن

sem_release(RW_SEM_ID); // آزاد کردن قفل
cprintf("Writer %d: finished writing\n", myproc()->pid);
return 0;
}