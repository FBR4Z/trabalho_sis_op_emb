// Controle com semáforo (POSIX)
// Simula sincronização entre threads (exemplo com 2 threads).
//
// Compilação e execução:
//   gcc semaphore_example.c -o semaphore_example -lpthread
//   ./semaphore_example

#include <stdio.h>
#include <pthread.h>   // pthread_create, pthread_join
#include <semaphore.h> // sem_t, sem_init, sem_wait, sem_post

// Declaração do semáforo (variável global para ser compartilhada entre threads)
sem_t sem;

// Função que cada thread executa
void* tarefa(void* arg) {
    // sem_wait(&sem):
    //   - decrementa o semáforo
    //   - se o valor for > 0, a thread continua
    //   - se o valor for 0, a thread fica BLOQUEADA até alguém liberar (sem_post)
    sem_wait(&sem);

    // Região crítica: só UMA thread por vez deve estar aqui
    printf("Thread %s executando\n", (char*)arg);

    // sem_post(&sem):
    //   - incrementa o semáforo
    //   - se havia threads bloqueadas, uma delas é acordada
    sem_post(&sem);

    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Inicializa o semáforo
    // sem_init(&sem, pshared, valor_inicial)
    //   pshared = 0 → semáforo usado entre threads (dentro do mesmo processo)
    //   valor_inicial = 1 → funciona como um "mutex" (apenas 1 thread de cada vez)
    sem_init(&sem, 0, 1);

    // Cria duas threads que executarão a função "tarefa"
    pthread_create(&t1, NULL, tarefa, "A");
    pthread_create(&t2, NULL, tarefa, "B");

    // Aguarda as duas threads terminarem
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Destroi o semáforo (boa prática)
    sem_destroy(&sem);

    return 0;
}