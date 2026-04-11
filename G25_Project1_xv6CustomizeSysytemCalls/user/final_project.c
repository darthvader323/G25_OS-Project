#include "kernel/types.h"
#include "user/user.h"

int main() {
    printf("--- xv6 Custom System Call & Process Test ---\n");
    
    // Testing your custom Syscall
    printf("System Year: %d\n", getyear());

    // Testing your Fork Modification
    int pid = fork();

    if(pid < 0) {
        printf("Fork failed!\n");
    } else if(pid == 0) {
        printf("Child process (PID %d) is running.\n", getpid());
        exit(0);
    } else {
        wait(0);
        printf("Parent process: Child %d has finished.\n", pid);
    }

    exit(0);
}
