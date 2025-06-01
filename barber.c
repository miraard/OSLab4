#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"
#include "semaphore.h"

#define BARBER_SEM_ID 3      // سمافر برای بیدار کردن آرایشگر
#define CHAIR_SEM_ID 4       // سمافر برای صندلی آرایش
#define WAITING_SEM_ID 5     // سمافر برای اتاق انتظار
#define MUTEX_SEM_ID 6       // سمافر برای دسترسی انحصاری
#define WAITING_ROOM_SIZE 3  // تعداد صندلی‌های اتاق انتظار

int waiting_customers = 0;   // تعداد مشتری‌های توی اتاق انتظار
struct spinlock barber_lock; // قفل برای لاگ‌ها

// تابع برای شبیه‌سازی تأخیر
void delay(int ticks) {
for (int i = 0; i < ticks * 100; i++) {
yield(); // کنترل رو به فرآیندهای دیگه بده
}
}

// مقداردهی اولیه سمافرها
int barber_init(void) {
initlock(&barber_lock, "barber_lock");
sem_init(BARBER_SEM_ID, 0);   // آرایشگر ابتدا خوابه (0)
sem_init(CHAIR_SEM_ID, 1);    // صندلی آرایش ابتدا خالیه (1)
sem_init(WAITING_SEM_ID, WAITING_ROOM_SIZE); // اتاق انتظار با ظرفیت 3
sem_init(MUTEX_SEM_ID, 1);    // قفل برای دسترسی انحصاری
waiting_customers = 0;
cprintf("barber_init: initialized with barber_sem_id=%d, chair_sem_id=%d, waiting_sem_id=%d, mutex_sem_id=%d\n",
BARBER_SEM_ID, CHAIR_SEM_ID, WAITING_SEM_ID, MUTEX_SEM_ID);
return 0;
}

// تابع آرایشگر
int barber(void) {
while (1) {
// آرایشگر منتظر مشتریه (می‌خوابه)
cprintf("Barber: sleeping, waiting for a customer\n");
sem_acquire(BARBER_SEM_ID); // منتظر بیدار شدن توسط مشتری

// آرایشگر بیدار شده و شروع به کار می‌کنه
sem_acquire(CHAIR_SEM_ID); // قفل صندلی آرایش رو بگیر
cprintf("Barber: cutting hair for a customer\n");
delay(20); // شبیه‌سازی زمان کوتاه کردن مو
cprintf("Barber: finished cutting hair\n");
sem_release(CHAIR_SEM_ID); // آزاد کردن صندلی آرایش

// بررسی مشتری‌های توی اتاق انتظار
sem_acquire(MUTEX_SEM_ID);
if (waiting_customers > 0) {
waiting_customers--;
cprintf("Barber: taking a customer from waiting room, remaining waiting customers=%d\n", waiting_customers);
sem_release(WAITING_SEM_ID); // یه صندلی توی اتاق انتظار آزاد کن
sem_release(MUTEX_SEM_ID);
sem_release(BARBER_SEM_ID); // آرایشگر رو دوباره بیدار کن (برای مشتری بعدی)
} else {
sem_release(MUTEX_SEM_ID);
}
}
return 0;
}

// تابع مشتری
int customer(void) {
// مشتری سعی می‌کنه وارد آرایشگاه بشه
sem_acquire(MUTEX_SEM_ID);
if (waiting_customers < WAITING_ROOM_SIZE) {
// جا توی اتاق انتظار هست
waiting_customers++;
cprintf("Customer %d: entered, waiting customers=%d\n", myproc()->pid, waiting_customers);
sem_acquire(WAITING_SEM_ID); // یه صندلی توی اتاق انتظار بگیر
sem_release(MUTEX_SEM_ID);

// بیدار کردن آرایشگر
sem_release(BARBER_SEM_ID);

// منتظر صندلی آرایش
sem_acquire(CHAIR_SEM_ID);
cprintf("Customer %d: getting haircut\n", myproc()->pid);
sem_release(CHAIR_SEM_ID); // بعد از اتمام کار، صندلی رو آزاد کن
} else {
// اتاق انتظار پره، مشتری می‌ره
cprintf("Customer %d: waiting room full, leaving\n", myproc()->pid);
sem_release(MUTEX_SEM_ID);
}
return 0;
}