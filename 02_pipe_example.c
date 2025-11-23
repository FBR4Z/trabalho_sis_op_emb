// Comunicação entre processos (pipe)
// Exemplo de IPC simples com pipe.
//
//  gcc pipe_example.c -o pipe_example
//  ./pipe_example

#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd[2];
    char buffer[50];

    pipe(fd); // cria o pipe

    if (fork() == 0) {
        // Processo filho - escreve no pipe
        close(fd[0]); // fecha leitura
        char msg[] = "Mensagem do filho!";
        write(fd[1], msg, strlen(msg)+1);
        close(fd[1]);
    } else {
        // Processo pai - lê do pipe
        close(fd[1]); // fecha escrita
        read(fd[0], buffer, sizeof(buffer));
        printf("Pai recebeu: %s\n", buffer);
        close(fd[0]);
    }
}