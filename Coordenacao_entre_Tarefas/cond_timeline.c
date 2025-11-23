// gcc -pthread cond_timeline.c -o cond_timeline
// ./cond_timeline
#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

/* ------------------------ Configuração do cenário ------------------------ */
#define N_CONSUMERS 3

/* ------------------------ Cores (desligue se quiser) --------------------- */
#define USE_COLOR 1
#define C_RESET   "\x1b[0m"
#define C_PROD    "\x1b[36m"   // ciano
#define C_BCAST   "\x1b[35m"   // magenta
#define C_CONS    "\x1b[32m"   // verde
#define C_MUTEX   "\x1b[33m"   // amarelo
#define C_WARN    "\x1b[31m"   // vermelho

/* ------------------------ Estado compartilhado --------------------------- */
static pthread_mutex_t mtx   = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
static int pronto = 0;  // condição lógica (0: nada pronto, 1: sinal, 2: broadcast)

/* ------------------------ Infra de logging com timestamp ----------------- */
static struct timespec t0;
static pthread_mutex_t log_mtx = PTHREAD_MUTEX_INITIALIZER;

static long long now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    long long ms = (ts.tv_sec - t0.tv_sec) * 1000LL +
                   (ts.tv_nsec - t0.tv_nsec) / 1000000LL;
    return ms;
}

static void logf(const char *color, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    pthread_mutex_lock(&log_mtx);

    if (USE_COLOR && color) fputs(color, stdout);
    printf("[%5lld ms] ", now_ms());
    vprintf(fmt, ap);
    if (USE_COLOR && color) fputs(C_RESET, stdout);
    fputc('\n', stdout);
    fflush(stdout);

    pthread_mutex_unlock(&log_mtx);
    va_end(ap);
}

/* ------------------------ Consumidores ----------------------------------- */
static void* consumidor(void *arg) {
    long id = (long) arg;

    logf(C_CONS, "C%ld: tentando lock(mtx)...", id);
    pthread_mutex_lock(&mtx);
    logf(C_MUTEX, "C%ld: lock adquirido.", id);

    while (!pronto) {
        logf(C_CONS, "C%ld: pronto==0 → WAIT (libera mtx e dorme)", id);
        pthread_cond_wait(&cond, &mtx); // libera mutex + dorme + reacquire
        logf(C_CONS, "C%ld: acordou de WAIT; pronto=%d (mtx já readquirido)", id, pronto);
    }

    logf(C_CONS, "C%ld: processando evento (pronto=%d)...", id, pronto);
    usleep(200 * 1000); // simula trabalho curto
    pthread_mutex_unlock(&mtx);
    logf(C_MUTEX, "C%ld: unlock(mtx) e fim.", id);
    return NULL;
}

/* ------------------------ Produtor: signal() ----------------------------- */
static void* produtor(void *arg) {
    (void)arg;
    usleep(800 * 1000); // dá tempo dos consumidores esperarem

    pthread_mutex_lock(&mtx);
    logf(C_PROD, "P : lock(mtx), ajustando pronto=1 e SIGNAL()");
    pronto = 1;
    pthread_cond_signal(&cond); // acorda apenas 1
    pthread_mutex_unlock(&mtx);
    logf(C_MUTEX, "P : unlock(mtx).");
    return NULL;
}

/* ------------------------ Broadcaster: broadcast() ----------------------- */
static void* broadcaster(void *arg) {
    (void)arg;
    usleep(1800 * 1000); // depois do produtor

    pthread_mutex_lock(&mtx);
    logf(C_BCAST, "B : lock(mtx), ajustando pronto=2 e BROADCAST()");
    pronto = 2;
    pthread_cond_broadcast(&cond); // acorda todas as remanescentes
    pthread_mutex_unlock(&mtx);
    logf(C_MUTEX, "B : unlock(mtx).");
    return NULL;
}

/* ------------------------ Main ------------------------------------------ */
int main(void) {
    clock_gettime(CLOCK_MONOTONIC, &t0);

    logf(NULL, "=== Demo: pthread_cond_wait / signal / broadcast (linha do tempo) ===");
    pthread_t cons[N_CONSUMERS], prod, bcast;

    for (long i = 0; i < N_CONSUMERS; ++i) {
        pthread_create(&cons[i], NULL, consumidor, (void*)i);
    }
    pthread_create(&prod,  NULL, produtor,    NULL);
    pthread_create(&bcast, NULL, broadcaster, NULL);

    for (int i = 0; i < N_CONSUMERS; ++i) pthread_join(cons[i], NULL);
    pthread_join(prod, NULL);
    pthread_join(bcast, NULL);

    logf(NULL, "=== Fim ===");
    return 0;
}
