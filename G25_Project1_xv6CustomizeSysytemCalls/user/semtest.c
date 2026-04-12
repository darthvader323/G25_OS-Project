#include "kernel/types.h"
#include "user/user.h"

int main() {
  printf("Semaphore Test Start\n");

  sem_init(1);

  int pid = fork();

  if(pid == 0){
    sem_wait();
    printf("Child entering critical section\n");
    sleep(5);
    printf("Child leaving critical section\n");
    sem_post();
    exit(0);
  } else {
    sem_wait();
    printf("Parent entering critical section\n");
    sleep(5);
    printf("Parent leaving critical section\n");
    sem_post();
    wait(0);
  }

  printf("Semaphore Test End\n");
  exit(0);
}