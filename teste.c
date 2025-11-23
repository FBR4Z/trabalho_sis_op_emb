#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    pid_t pid = getpid();
    
    printf("PID do processo: %d\n", pid);
    printf("Testando fork()...\n");
    
    pid_t filho = fork();
    
    if (filho == 0) {
        printf("Sou o processo FILHO! PID: %d\n", getpid());
    } else {
        printf("Sou o processo PAI! PID: %d, filho: %d\n", getpid(), filho);
    }
    
    return 0;
}