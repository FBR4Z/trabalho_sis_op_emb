// Comunicação entre processos (pipe)
// Exemplo de IPC (Inter-Process Communication) simples com pipe.
//
// Compilação e execução:
//   gcc pipe_example.c -o pipe_example
//   ./pipe_example

#include <stdio.h>    // printf
#include <unistd.h>   // fork, pipe, read, write, close
#include <string.h>   // strlen

int main() {
    int fd[2];          // descritores de arquivo do pipe: fd[0] = leitura, fd[1] = escrita
    char buffer[50];    // buffer para armazenar a mensagem lida

    // Cria um pipe anônimo (só existe enquanto os processos estiverem vivos).
    // pipe(fd) preenche fd[0] (leitura) e fd[1] (escrita).
    if (pipe(fd) == -1) {
        perror("Erro ao criar pipe");
        return 1;
    }

    // Cria um processo filho (duplica o processo atual).
    if (fork() == 0) {
        // ================================
        // BLOCO DO FILHO
        // ================================
        close(fd[0]); // o filho não vai ler, então fecha o descritor de leitura

        char msg[] = "Mensagem do filho!";
        // escreve no pipe: vai para fd[1]
        // strlen(msg)+1 inclui o '\0' no final da string
        write(fd[1], msg, strlen(msg)+1);

        close(fd[1]); // fecha o descritor de escrita (boa prática)
    } else {
        // ================================
        // BLOCO DO PAI
        // ================================
        close(fd[1]); // o pai não vai escrever, então fecha o descritor de escrita

        // lê do pipe (bloqueia até o filho escrever algo)
        read(fd[0], buffer, sizeof(buffer));

        // imprime a mensagem recebida do filho
        printf("Pai recebeu: %s\n", buffer);

        close(fd[0]); // fecha o descritor de leitura
    }
}