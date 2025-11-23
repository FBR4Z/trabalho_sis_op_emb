/*
 * =========================================================================
 * EXEMPLO: Consumidor de mensagens com Fila de Mensagens POSIX (mq_*)
 * =========================================================================
 * Arquivo: mq-recv.c
 *
 * Conceitos:
 *   - Fila de mensagens POSIX (mqueue): comunicação entre processos
 *   - mq_open / mq_receive / mq_close / mq_unlink
 *
 * Compilação (Linux):
 *   gcc mq-recv.c -o mq-recv -lrt
 *   (em glibc mais nova pode não precisar de -lrt, mas mantemos por didática)
 *
 * Execução:
 *   ./mq-recv
 * =========================================================================
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>   // para permissões (mode 0666)
#include <fcntl.h>      // para O_CREAT, O_RDWR, etc.

#define QUEUE "/my_queue"    // nome da fila POSIX (precisa começar com '/')

int main (int argc, char *argv[])
{
    mqd_t queue;        // descritor da fila de mensagens (message queue descriptor)
    struct mq_attr attr; // atributos da fila de mensagens
    int msg;            // cada mensagem será um número inteiro

    /* ------------------------------------------------------------
     * 1. Definir os atributos da fila de mensagens
     * ------------------------------------------------------------
     * mq_maxmsg  → capacidade máxima de mensagens simultâneas
     * mq_msgsize → tamanho, em bytes, de CADA mensagem
     * mq_flags   → 0 = operações BLOQUEANTES (esperam mensagem chegar)
     */
    attr.mq_maxmsg  = 10;            // fila pode armazenar até 10 mensagens
    attr.mq_msgsize = sizeof(msg);   // cada mensagem tem o tamanho de um int
    attr.mq_flags   = 0;             // bloqueante (mq_receive espera se fila estiver vazia)

    /* ------------------------------------------------------------
     * 2. Criar ou abrir a fila de mensagens
     * ------------------------------------------------------------
     * mq_open(nome, flags, mode, attr)
     *   O_RDWR  → abre para leitura e escrita
     *   O_CREAT → cria se não existir
     *   0666    → permissões de leitura/escrita para todos (rw-rw-rw-)
     */
    queue = mq_open(QUEUE, O_RDWR | O_CREAT, 0666, &attr);
    if (queue == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    printf("=== Consumidor: aguardando mensagens na fila %s ===\n", QUEUE);

    /* ------------------------------------------------------------
     * 3. Laço principal: receber e imprimir mensagens
     * ------------------------------------------------------------
     * mq_receive(queue, buffer, tamanho, prioridade)
     *   - bloqueia até chegar uma mensagem
     *   - copia a mensagem para 'msg'
     */
    for (;;) {
        if (mq_receive(queue, (void *)&msg, sizeof(msg), 0) < 0) {
            perror("mq_receive");
            exit(1);
        }
        printf("Consumidor recebeu valor: %d\n", msg);
    }

    // (nunca chega aqui nesse exemplo, pois o laço é infinito)
    // mq_close(queue);
    // mq_unlink(QUEUE); // remover a fila do sistema

    return 0;
}
