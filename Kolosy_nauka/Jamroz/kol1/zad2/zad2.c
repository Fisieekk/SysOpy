#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int fd[2];

void sigusr1_handler(int signum) {
  int value;
  read(fd[0], &value, sizeof(int));
  write(fd[1], &value, sizeof(int));
}

int main(int argc, char *argv[]) {
  pid_t pid1, pid2;
  int value = 0;

  if (pipe(fd) == -1) {
    perror("pipe");
    return 1;
  }

  pid1 = fork();
  if (pid1 < 0) {
    perror("fork");
    return 1;
  } else if (pid1 == 0) { // child process 1
    close(fd[1]);
    signal(SIGUSR1, sigusr1_handler);
    while (1) {
      pause();
    }
    return 0;
  } else {
    pid2 = fork();
    if (pid2 < 0) {
      perror("fork");
      return 1;
    } else if (pid2 == 0) { // child process 2
      close(fd[0]);
      while (1) {
        read(fd[1], &value, sizeof(int));
        printf("Received value: %d\n", value);
      }
      return 0;
    }
  }

  close(fd[0]);
  close(fd[1]);

  while (1) {
    sleep(1);
    kill(pid1, SIGUSR1);
    write(fd[0], &value, sizeof(int));
    value++;
  }

  return 0;
}
