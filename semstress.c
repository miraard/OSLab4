#include "types.h"
#include "stat.h"
#include "user.h"

struct semaphore sem;

void
worker(int id)
{
  for(int i = 0; i < 5; i++) {
    printf(1, "Worker %d: trying to acquire semaphore (iter %d)...\n", id, i);
    sem_acquire(&sem);
    printf(1, "Worker %d: acquired semaphore (iter %d)\n", id, i);
    sleep(10 * (id % 3 + 1)); // زمان تصادفی برای شبیه‌سازی کار
    printf(1, "Worker %d: releasing semaphore (iter %d)...\n", id, i);
    sem_release(&sem);
  }
  exit();
}

int
main(int argc, char *argv[])
{
  sem_init(&sem, 3); // Semaphore با مقدار اولیه 3

  printf(1, "Creating 10 workers...\n");
  for(int i = 0; i < 10; i++) {
    if(fork() == 0) {
      worker(i);
    }
  }

  for(int i = 0; i < 10; i++) {
    wait();
  }

  printf(1, "Stress test finished.\n");
  exit();
}