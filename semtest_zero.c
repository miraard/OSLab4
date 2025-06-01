#include "types.h"
#include "stat.h"
#include "user.h"
#include "semaphore.h"

int sem_id; // متغیر سراسری برای ID

void waiter(void) {
printf(1, "Waiter: trying to acquire semaphore...\n");
sem_acquire(sem_id);
printf(1, "Waiter: acquired semaphore!\n");
sleep(50);
printf(1, "Waiter: releasing semaphore...\n");
sem_release(sem_id);
printf(1, "Waiter: finished.\n");
exit();
}

void signaler(void) {
printf(1, "Signaler: starting, pid = %d\n", getpid());
sleep(200);
printf(1, "Signaler: releasing semaphore...\n");
sem_release(sem_id);
printf(1, "Signaler: finished.\n");
exit();
}

int main(int argc, char *argv[]) {
sem_id = 0; // استفاده از ID 0
if (sem_init(sem_id, 0) < 0) {
printf(1, "Main: failed to initialize semaphore\n");
exit();
}

printf(1, "Creating waiter...\n");
if(fork() == 0) {
waiter();
}

sleep(20);

printf(1, "Creating signaler...\n");
if(fork() == 0) {
signaler();
}

wait();
wait();

printf(1, "Test finished.\n");
exit();
}