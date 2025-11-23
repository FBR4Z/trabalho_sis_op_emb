/*
 * =========================================================================
 * EXEMPLO: Produtor de mensagens com Fila de Mensagens POSIX (mq_*)
 * =========================================================================
 * Arquivo: mq-send.c
 *
 * Conceitos:
 *   - Uso de mq_open para abrir fila existente
 *   - Envio de mensagens com mq_send
 *   - Vários produtores podem escrever na MESMA fila
 *
 * Compilação (Linux):
 *   gcc mq-send.c -o mq-send -lrt
 *
 * Execução (depois de rodar o consumidor):
 *   ./mq-send
 * =========================================================================
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <unistd.h>     // sleep()
#include <fcntl.h>      // O_RDWR

#define QUEUE "/my_queue"    // mesmo nome usado no consumidor

int main (int argc, char *argv[])
{
    mqd_t queue;   // descritor para a fila de mensagens
    int   msg;     // mensagem (inteiro) que será enviada

    /* ------------------------------------------------------------
     * 1. Abrir a fila de mensagens já existente
     * ------------------------------------------------------------
     * IMPORTANTE:
     *   - aqui não usamos O_CREAT; assumimos que o consumidor já
     *     criou a fila com mq_open(..., O_CREAT, ...).
     */
    queue = mq_open(QUEUE, O_RDWR);
    if (queue == (mqd_t)-1) {
        perror("mq_open");
        fprintf(stderr,
                "Dica: execute primeiro o consumidor (mq-recv), "
                "pois é ele quem cria a fila '%s'.\n", QUEUE);
        exit(1);
    }

    printf("=== Produtor: enviando mensagens para a fila %s ===\n", QUEUE);

    /* ------------------------------------------------------------
     * 2. Loop infinito: gera e envia mensagens
     * ------------------------------------------------------------
     */
    for (;;) {
        // número aleatório entre 0 e 99
        msg = random() % 100;

        // Envia a mensagem:
        // mq_send(queue, buffer, tamanho, prioridade)
        if (mq_send(queue, (void *)&msg, sizeof(msg), 0) < 0) {
            perror("mq_send");
            exit(1);
        }

        printf("Produtor enviou valor: %d\n", msg);

        // Pausa de 1 segundo entre envios (para visualização mais clara)
        sleep(1);
    }

    // (nunca chega aqui neste exemplo)
    // mq_close(queue);

    return 0;
}
