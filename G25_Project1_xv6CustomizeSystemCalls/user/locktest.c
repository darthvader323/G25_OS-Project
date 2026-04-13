#include "kernel/types.h"
#include "user/user.h"

int main() {
  int lock = lock_create();

  if(lock < 0) {
    printf("Lock creation failed\n");
    exit(1);
  }

  printf("Trying to acquire lock...\n");

  lock_acquire(lock);
  printf("Inside critical section\n");

  for(int i = 0; i < 5; i++) {
    printf("Working %d\n", i);
  }

  lock_release(lock);
  printf("Lock released\n");

  exit(0);
}
