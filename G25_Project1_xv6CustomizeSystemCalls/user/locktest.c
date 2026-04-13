#include "kernel/types.h"
#include "user/user.h"

int main() {
  int lock = lock_create();

  if(lock < 0) {
    printf("Lock creation failed\n");
    exit(1);
  }

  int pid = fork();

  if(pid == 0) {
    // Child process
    lock_acquire(lock);
    printf("Child acquired lock\n");

    for(int i = 0; i < 5; i++) {
      printf("Child working %d\n", i);
    }

    lock_release(lock);
    printf("Child released lock\n");
    exit(0);
  } else {
    // Parent process
    sleep(10);

    lock_acquire(lock);
    printf("Parent acquired lock\n");

    for(int i = 0; i < 5; i++) {
      printf("Parent working %d\n", i);
    }

    lock_release(lock);
    printf("Parent released lock\n");

    wait(0);
  }

  exit(0);
}
