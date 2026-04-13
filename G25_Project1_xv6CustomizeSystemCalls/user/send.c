#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if(argc < 3){
    printf("Usage: send pid message\n");
    exit(1);
  }

  int pid = atoi(argv[1]);
  send(pid, argv[2]);

  exit(0);
}
