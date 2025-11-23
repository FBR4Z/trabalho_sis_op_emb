/*
 * ===========================================
 * MONITOR EM C (POSIX THREADS)
 * ===========================================
 * Objetivo:
 *   Demonstrar o uso de mutex + variável de condição (cond_var)
 *   para criar um "monitor" que controla o acesso ao recurso compartilhado.
 *
 * Conceitos usados:
 *   - pthread_mutex_t  → exclusão mútua
 *   - pthread_cond_t   → espera e sinalização de eventos
 *   - struct Conta     → encapsula dados e mecanismos de sincronização
 *
 * Compilar:
 *   gcc -O2 -pthread conta_monitor.c -o conta_monitor
 * Executar:
 *   ./conta_monitor
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* ----------------------------------------------------------
 * Estrutura de dados do "monitor" Conta
 * ---------------------------------------------------------- */
typedef struct {
    double saldo;                // recurso compartilhado
    pthread_mutex_t mtx;         // mutex interno (exclusão mútua)
    pthread_cond_t cond;         // variável de condição (sincronização)
} Conta;

/* ----------------------------------------------------------
 * Inicializa e destrói a conta
 * ---------------------------------------------------------- */
void conta_init(Conta *c, double saldo_inicial) {
    c->saldo = saldo_inicial;
    pthread_mutex_init(&c->mtx, NULL);
    pthread_cond_init(&c->cond, NULL);
}

void conta_destroy(Conta *c) {
    pthread_mutex_destroy(&c->mtx);
    pthread_cond_destroy(&c->cond);
}

/* ----------------------------------------------------------
 * Operação: depositar
 * ----------------------------------------------------------
 * - Obtém o mutex;
 * - Atualiza o saldo;
 * - Sinaliza (wake-up) possíveis threads bloqueadas em "retirar".
 */
void conta_depositar(Conta *c, double valor) {
    pthread_mutex_lock(&c->mtx);
    c->saldo += valor;
    printf("[Depositar] +%.2f → Saldo = %.2f\n", valor, c->saldo);
    pthread_cond_signal(&c->cond);  // acorda uma thread esperando saldo
    pthread_mutex_unlock(&c->mtx);
}

/* ----------------------------------------------------------
 * Operação: retirar
 * ----------------------------------------------------------
 * - Obtém o mutex;
 * - Se não há saldo suficiente → entra em "wait" (libera mutex e bloqueia);
 * - Quando acorda, reavalia a condição (while!);
 * - Atualiza saldo e libera mutex.
 */
void conta_retirar(Conta *c, double valor) {
    pthread_mutex_lock(&c->mtx);
    while (c->saldo < valor) {  // evita wakeups indevidos (semântica Mesa)
        printf("[Retirar] Aguardando saldo suficiente... (Saldo = %.2f)\n", c->saldo);
        pthread_cond_wait(&c->cond, &c->mtx);
    }
    c->saldo -= valor;
    printf("  [Retirar] -%.2f → Saldo = %.2f\n", valor, c->saldo);
    pthread_mutex_unlock(&c->mtx);
}

/* ----------------------------------------------------------
 * Threads simulando clientes (produtor/consumidor)
 * ---------------------------------------------------------- */
void *cliente_depositar(void *arg) {
    Conta *c = (Conta *)arg;
    for (int i = 0; i < 5; i++) {
        conta_depositar(c, 100);
        usleep(150000);  // 150 ms
    }
    return NULL;
}

void *cliente_retirar(void *arg) {
    Conta *c = (Conta *)arg;
    for (int i = 0; i < 5; i++) {
        conta_retirar(c, 120);
        usleep(200000);  // 200 ms
    }
    return NULL;
}

/* ----------------------------------------------------------
 * Função principal
 * ---------------------------------------------------------- */
int main(void) {
    Conta c;
    conta_init(&c, 50);  // saldo inicial

    pthread_t t1, t2;
    pthread_create(&t1, NULL, cliente_depositar, &c);
    pthread_create(&t2, NULL, cliente_retirar, &c);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\n[MAIN] Saldo final: %.2f\n", c.saldo);

    conta_destroy(&c);
    return 0;
}
