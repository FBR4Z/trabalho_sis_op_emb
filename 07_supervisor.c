/*
 * supervisor.c
 * Pai cria filhos e monitora seus estados em tempo real
 *
 * Compilação:
 *   gcc supervisor.c -o supervisor
 *
 * Execução:
 *   ./supervisor
 *
 * Em outro terminal, você pode enviar sinais:
 *   kill -STOP <PID_DO_FILHO>   // coloca filho em T (Stopped)
 *   kill -CONT <PID_DO_FILHO>   // retoma filho
 *   kill -TERM <PID_DO_FILHO>   // encerra filho (vai gerar evento)
 */

 // ADICIONE ESTA LINHA AQUI:
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2;

    // Cria primeiro filho
    pid1 = fork();
    if (pid1 == 0) {
        // Código do filho 1: loop infinito
        while (1) {
            pause(); // aguarda sinais
        }
        exit(0);
    }

    // Cria segundo filho
    pid2 = fork();
    if (pid2 == 0) {
        // Código do filho 2: loop infinito
        while (1) {
            pause(); // aguarda sinais
        }
        exit(0);
    }

    printf("Pai supervisor PID=%d criou filhos: %d e %d\n", getpid(), pid1, pid2);

    // Loop de supervisão
    while (1) {
        int status;
        pid_t w = waitpid(-1, &status, WUNTRACED | WCONTINUED);

        if (w == -1) {
            perror("waitpid");
            exit(1);
        }

        if (WIFEXITED(status)) {
            printf("Filho %d terminou normalmente com código %d\n", w, WEXITSTATUS(status));
        } 
        else if (WIFSIGNALED(status)) {
            printf("Filho %d morreu por sinal %d\n", w, WTERMSIG(status));
        } 
        else if (WIFSTOPPED(status)) {
            printf("Filho %d foi PARADO (SIGSTOP ou Ctrl+Z), sinal=%d\n", w, WSTOPSIG(status));
        } 
        else if (WIFCONTINUED(status)) {
            printf("Filho %d foi CONTINUADO (SIGCONT)\n", w);
        }
    }

    return 0;
}