#include "types.h"
#include "stat.h"
#include "user.h"

struct semaphore sem;

void
worker(int id)
{
  printf(1, "Worker %d: trying to acquire semaphore...\n", id);
  sem_acquire(&sem);
  printf(1, "Worker %d: acquired semaphore!\n", id);
  sleep(50 * (id + 1)); // هر worker مدت متفاوتی صبر می‌کنه
  printf(1, "Worker %d: releasing semaphore...\n", id);
  sem_release(&sem);
  exit();
}

int
main(int argc, char *argv[])
{
  sem_init(&sem, 2); // Semaphore با مقدار اولیه 2

  printf(1, "Creating 4 workers...\n");
  for(int i = 0; i < 4; i++) {
    if(fork() == 0) {
      worker(i);
    }
  }

  for(int i = 0; i < 4; i++) {
    wait();
  }

  printf(1, "All workers finished.\n");
  exit();
}