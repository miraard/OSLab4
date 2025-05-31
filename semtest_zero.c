#include "types.h"
#include "stat.h"
#include "user.h"

struct semaphore sem;
void waiter(void) {
    printf(1, "Waiter: trying to acquire semaphore...\n");
    sem_acquire(&sem);
    printf(1, "Waiter: acquired semaphore!\n");
    sleep(50);
    printf(1, "Waiter: releasing semaphore...\n");
    sem_release(&sem);
    exit();
}

void signaler(void) {
    printf(1, "Signaler: starting, pid = %d\n", getpid());
    sleep(100); // افزایش تأخیر برای اطمینان از خواب رفتن waiter
    printf(1, "Signaler: releasing semaphore...\n");
    sem_release(&sem);
    exit();
}

int main(int argc, char *argv[]) {
    sem_init(&sem, 0); // Semaphore با مقدار اولیه 0

    printf(1, "Creating waiter...\n");
    if(fork() == 0) {
        waiter();
    }

    sleep(10); // تأخیر بیشتر برای اطمینان از اجرای waiter

    printf(1, "Creating signaler...\n");
    if(fork() == 0) {
        signaler();
    }

    wait();
    wait();

    printf(1, "Test finished.\n");
    exit();
}