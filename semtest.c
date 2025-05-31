#include "types.h"
#include "stat.h"
#include "user.h"

struct semaphore sem;

void
child(void)
{
  printf(1, "Child trying to acquire semaphore...\n");
  sem_acquire(&sem);
  printf(1, "Child acquired semaphore!\n");
  sleep(100); // کمی صبر می‌کنه
  printf(1, "Child releasing semaphore...\n");
  sem_release(&sem);
  exit();
}

int
main(int argc, char *argv[])
{
  sem_init(&sem, 1); // Semaphore با مقدار اولیه 1

  printf(1, "Parent creating child...\n");
  if(fork() == 0) {
    child();
  } else {
    printf(1, "Parent trying to acquire semaphore...\n");
    sem_acquire(&sem);
    printf(1, "Parent acquired semaphore!\n");
    sleep(50);
    printf(1, "Parent releasing semaphore...\n");
    sem_release(&sem);
    wait();
  }

  printf(1, "Test finished.\n");
  exit();
}