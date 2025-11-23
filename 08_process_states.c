/*
 * process_states.c
 * Demonstra processos nos estados R, S, Z e T
 *
 * Compilação:
 *   gcc process_states.c -o process_states
 *
 * Execução:
 *   ./process_states
 * Em outro terminal:
 *   ps aux | grep process_states
 *   Use 'ps -o pid,stat,comm -p %d,%d,%d,%d' para ver os estados.
 *   top -H -p <PID>
 * 
 * Explicação: 
 * 
 * RUNNING (R): filho 1 faz loop infinito → sempre usando CPU ou pronto para usar.
 * SLEEPING (S): filho 2 chama sleep(10) repetidamente → entra em estado S.
 * ZOMBIE (Z): filho 3 termina, mas o pai não chama wait() → permanece como zumbi.
 * STOPPED (T): filho 4 é parado com SIGSTOP.
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main() {
    pid_t pidR, pidS, pidZ, pidT;

    printf("PID do programa principal: %d\n", getpid());

    // -----------------------------
    // 1. Processo em RUNNING (R)
    // -----------------------------
    pidR = fork();
    if (pidR == 0) {
        // Filho 1: laço infinito (sempre R ou pronto para rodar)
        while (1) {
            // pequeno trabalho para ocupar CPU
        }
        exit(0);
    }

    // -----------------------------
    // 2. Processo em SLEEPING (S)
    // -----------------------------
    pidS = fork();
    if (pidS == 0) {
        // Filho 2: dorme continuamente
        while (1) {
            sleep(10); // estado "S" (esperando)
        }
        exit(0);
    }

    // -----------------------------
    // 3. Processo em ZOMBIE (Z)
    // -----------------------------
    pidZ = fork();
    if (pidZ == 0) {
        printf("Filho Z (vai virar zumbi) PID=%d\n", getpid());
        exit(0); // termina imediatamente
    } else {
        sleep(1); // pai NÃO chama wait() → filho fica em estado Z
    }

    // -----------------------------
    // 4. Processo em STOPPED (T)
    // -----------------------------
    pidT = fork();
    if (pidT == 0) {
        while (1) {
            pause(); // espera por sinais
        }
        exit(0);
    } else {
        sleep(1); // garante que filho está em execução
        kill(pidT, SIGSTOP); // envia SIGSTOP → processo fica em T
    }

    // -----------------------------
    // Mantém o pai vivo para observar no ps/top
    // -----------------------------
    printf("Processos criados: R=%d  S=%d  Z=%d  T=%d\n",
           pidR, pidS, pidZ, pidT);

    printf("Use 'ps -o pid,stat,comm -p %d,%d,%d,%d' para ver os estados.\n",
           pidR, pidS, pidZ, pidT);

    while (1) {
        sleep(30);
    }

    return 0;
}