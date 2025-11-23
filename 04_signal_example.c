// Tratando sinais (interrupções no espaço do usuário)
// Simula interrupções com signal handling.

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void handler(int sig) {
    printf("Recebi sinal %d! Encerrando programa...\n", sig);
    exit(0); // encerra o programa
}

int main() {
    signal(SIGINT, handler); // captura Ctrl+C
    while (1) {
        printf("Executando... pressione Ctrl+C\n");
        sleep(1);
    }
    return 0;
}