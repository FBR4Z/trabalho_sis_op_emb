/*
 * Exemplo didático – Variáveis de condição (pthread_cond_t)
 *
 * Demonstra o funcionamento das operações:
 *   - wait()   → pthread_cond_wait()
 *   - signal() → pthread_cond_signal()
 *   - broadcast() → pthread_cond_broadcast()
 *
 * Situação: uma thread "produtora" sinaliza quando um dado está pronto.
 * Outras threads "consumidoras" esperam até o sinal.
 *
 * Compilar:
 *   gcc -pthread cond_wait_signal.c -o cond_wait_signal
 *
 * Executar:
 *   ./cond_wait_signal
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* -------------------------------------------------------------
 * Variáveis globais compartilhadas entre threads
 * ------------------------------------------------------------- */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // protege o acesso a 'pronto'
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;   // variável de condição
int pronto = 0;                                   // condição lógica compartilhada

/* -------------------------------------------------------------
 * Thread CONSUMIDORA
 * ------------------------------------------------------------- */
void* consumidor(void* arg) {
    long id = (long)arg;

    pthread_mutex_lock(&mutex);

    // Enquanto a condição não for verdadeira, aguarda o sinal
    while (!pronto) {
        printf("Consumidor %ld aguardando...\n", id);
        // Libera mutex + suspende thread (operações atômicas)
        pthread_cond_wait(&cond, &mutex);
    }

    printf("Consumidor %ld acordou! pronto=%d\n", id, pronto);

    pthread_mutex_unlock(&mutex);
    return NULL;
}

/* -------------------------------------------------------------
 * Thread PRODUTORA
 * ------------------------------------------------------------- */
void* produtor(void* arg) {
    (void)arg;
    sleep(2); // simula tempo de processamento

    pthread_mutex_lock(&mutex);
    pronto = 1;
    printf("\nProdutor: pronto=%d → enviando sinal (pthread_cond_signal)\n", pronto);

    // Acorda uma thread esperando
    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&mutex);
    return NULL;
}

/* -------------------------------------------------------------
 * Thread BROADCASTER (acorda todas as threads)
 * ------------------------------------------------------------- */
void* broadcaster(void* arg) {
    (void)arg;
    sleep(4);

    pthread_mutex_lock(&mutex);
    pronto = 2;
    printf("\nBroadcaster: pronto=%d → acordando TODAS (pthread_cond_broadcast)\n", pronto);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

/* -------------------------------------------------------------
 * Função principal
 * ------------------------------------------------------------- */
int main(void) {
    pthread_t consumidores[3], tprod, tbcast;

    // Cria 3 threads consumidoras
    for (long i = 0; i < 3; i++) {
        pthread_create(&consumidores[i], NULL, consumidor, (void*)i);
    }

    // Cria produtor e broadcaster
    pthread_create(&tprod, NULL, produtor, NULL);
    pthread_create(&tbcast, NULL, broadcaster, NULL);

    // Aguarda todas finalizarem
    for (int i = 0; i < 3; i++) pthread_join(consumidores[i], NULL);
    pthread_join(tprod, NULL);
    pthread_join(tbcast, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    puts("\nFim da execução.");
    return 0;
}
