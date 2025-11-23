// Preempção por fatia de tempo (time slice) – sem sleep, sem yield
// Dois threads CPU-bound, cada um tentando monopolizar a CPU. Mesmo assim, 
// ambos “andam” porque o kernel preempteia (SCHED_OTHER).
// 
// Para compilar e executar
// gcc preempt_timeslice.c -o preempt_timeslice -lpthread
// ./preempt_timeslice

#include <stdio.h>      // printf
#include <pthread.h>    // pthread_create, pthread_t
#include <stdatomic.h>  // variáveis atômicas (seguras para concorrência)
#include <time.h>       // nanosleep

// Dois contadores globais, um para cada thread.
// "atomic_ulong" garante que as operações são atômicas,
// evitando resultados inconsistentes em ambiente multithread.
atomic_ulong c1 = 0, c2 = 0;

// Função que será executada pela primeira thread.
// Essa thread entra em um loop infinito, incrementando "c1".
void* t1(void* _) {
    for (;;) {
        c1++;
    }
}

// Função que será executada pela segunda thread.
// Essa thread também entra em loop infinito, incrementando "c2".
void* t2(void* _) {
    for (;;) {
        c2++;
    }
}

int main() {
    pthread_t a, b;

    // Criamos duas threads, ambas com a mesma prioridade e sem "sleep" ou "yield".
    // Cada thread tenta monopolizar a CPU, mas o escalonador do Linux
    // aplica preempção por fatia de tempo (time slice).
    pthread_create(&a, NULL, t1, NULL);
    pthread_create(&b, NULL, t2, NULL);

    // O processo principal imprime os valores dos contadores a cada 300 ms.
    for (int i = 0; i < 10; i++) {
        struct timespec ts = { .tv_sec = 0, .tv_nsec = 300*1000*1000 }; // 300 ms
        nanosleep(&ts, NULL);  // pausa só no processo principal, não nas threads!

        unsigned long x = c1, y = c2;
        printf("t1=%lu  t2=%lu\n", x, y);
    }

    return 0;
}