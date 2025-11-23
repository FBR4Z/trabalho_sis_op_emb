/*
 * =========================================================================
 * EXEMPLO: Comunicação Pai → Filho usando PIPE em C (POSIX)
 * =========================================================================
 * Objetivo:
 *   Demonstrar como criar um pipe, criar um processo filho com fork(),
 *   e transmitir dados do processo PAI para o processo FILHO.
 *
 * Conceitos envolvidos:
 *   - pipe(fd): cria um canal unidirecional fd[0] = leitura, fd[1] = escrita
 *   - fork(): cria processo filho
 *   - write(): escreve no pipe
 *   - read(): lê do pipe
 *   - close(): fecha extremidades não usadas (obrigatório!)
 *
 * Fluxo:
 *   1. Pai fecha fd[0] → só escreve
 *   2. Filho fecha fd[1] → só lê
 *   3. Pai envia string
 *   4. Filho recebe string e imprime
 *  
 * Compilar:
 *   gcc mypipe.c -o mypipe
 *
 * Executar:
 *   ./mypipe
 * =========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER 256   // tamanho máximo usado para strings no pipe

int main(void) {

    int fd[2];       // fd[0] = leitura, fd[1] = escrita
    pid_t pid;       // variável para armazenar o PID do fork

    /* ------------------------------------------------------------
     * 1. Criando o PIPE
     * ------------------------------------------------------------
     * pipe(fd) inicializa o vetor fd da seguinte forma:
     *   fd[0] → extremidade de LEITURA
     *   fd[1] → extremidade de ESCRITA
     */
    if (pipe(fd) < 0) {
        perror("Erro ao criar pipe");
        return -1;
    }

    /* ------------------------------------------------------------
     * 2. Criando o processo FILHO via fork()
     * ------------------------------------------------------------
     *  fork() retorna:
     *   > 0 → estamos no processo PAI (valor é o PID do filho)
     *   = 0 → estamos no processo FILHO
     *   < 0 → erro
     */
    if ((pid = fork()) < 0) {
        perror("Erro ao criar processo com fork");
        exit(1);
    }

    /* ============================================================
     * 3. PROCESSO PAI  (pid > 0)
     * ============================================================ */
    if (pid > 0)
    {
        // Pai vai *escrever* → fecha o lado de leitura
        close(fd[0]);

        // Mensagem para enviar ao filho
        char str[BUFFER] = "Aprendi a usar Pipes em C!\n";

        printf("PAI enviando pelo PIPE: '%s'\n", str);

        // Enviando a string pelo pipe
        write(fd[1], str, sizeof(str));

        exit(0);
    }

    /* ============================================================
     * 4. PROCESSO FILHO (pid == 0)
     * ============================================================ */
    else
    {
        char str_recebida[BUFFER];

        // Filho vai *ler* → fecha o lado de escrita
        close(fd[1]);

        // Lendo dados enviados pelo pai
        read(fd[0], str_recebida, sizeof(str_recebida));

        printf("FILHO recebeu pelo PIPE: '%s'\n", str_recebida);

        exit(0);
    }

    return 0;
}
