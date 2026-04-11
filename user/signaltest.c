#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sigterm_handler(int signum) {
  printf("Child: received signal %d, running handler!\n", signum);
  sigreturn();
}

void sigusr1_handler(int signum) {
  printf("Child: SIGUSR1 (%d) received!\n", signum);
  sigreturn();
}

int main(void) {
  printf("\n=== xv6 Signal Demo ===\n");

  signal(15, sigterm_handler);
  signal(10, sigusr1_handler);

  int pid = fork();

  if(pid == 0) {
    // Child process
    printf("Child PID=%d: registering handlers and sleeping...\n", getpid());
    signal(15, sigterm_handler);
    signal(10, sigusr1_handler);
    sleep(20);
    printf("Child: woke up normally.\n");
    exit(0);
  } else {
    // Parent process
    sleep(2);
    printf("Parent: sending SIGUSR1 (10) to child PID=%d\n", pid);
    kill(pid, 10);

    sleep(2);
    printf("Parent: sending SIGTERM (15) to child PID=%d\n", pid);
    kill(pid, 15);

    wait(0);
    printf("=== Signal Demo Complete ===\n\n");
    exit(0);
  }
}
