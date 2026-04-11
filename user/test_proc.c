#include "kernel/types.h"
#include "user/user.h"

int main() {
  printf("Starting Process Creation Test...\n");
  
  int pid = fork();
  
  if(pid < 0){
    printf("Fork failed\n");
    exit(1);
  } else if(pid == 0){
    printf("Child Process: Hello from PID %d\n", getpid());
    exit(0);
  } else {
    printf("Parent Process: Created child with PID %d\n", pid);
    wait(0);
    printf("Child finished. Parent exiting.\n");
  }
  
  exit(0);
}
