// Exemplo de uso de threads POSIX em C no Linux
// Compilar:  gcc ex_threads.c -o ex_threads -lpthread
// ./ex_threads

#include <pthread.h>   // pthread_create, pthread_join, pthread_exit...
#include <stdio.h>     // printf, perror
#include <stdlib.h>    // exit, malloc, free
#include <unistd.h>    // sleep

#define NUM_THREADS 5  // número total de threads que criaremos

// ---------------------------------------------------------------------
// Função executada por cada thread.
// O parâmetro é void* (convenção POSIX). Aqui esperamos receber long*.
// ---------------------------------------------------------------------
void* print_hello(void* arg)
{
    // Converte o argumento genérico para o tipo esperado (ponteiro para long)
    long id = *(long*)arg;

    // Mensagem de início da thread
    printf("%ld: Hello World!\n", id);

    // Simula algum "trabalho" (I/O, cálculo, etc.)
    sleep(5);

    // Mensagem final da thread
    printf("%ld: Bye bye World!\n", id);

    // Importante: a função de thread deve retornar via pthread_exit ou return
    pthread_exit(NULL);
    // (return NULL; // equivaleria aqui)
}

// ---------------------------------------------------------------------
// Função principal: cria e sincroniza as threads.
// ---------------------------------------------------------------------
int main(void)
{
    pthread_t th[NUM_THREADS];  // array com os IDs das threads criadas
    long* ids = malloc(sizeof(long) * NUM_THREADS);
    if (!ids) {
        perror("malloc");
        exit(1);
    }

    // 1) Criar as threads
    for (long i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;  // guarda valor de ID (um por thread)

        // pthread_create:
        //  - &th[i]  : onde o ID da thread será armazenado
        //  - NULL    : atributos default
        //  - print_hello : função que a thread vai executar
        //  - &ids[i] : argumento passado à função (ponteiro para long)
        int rc = pthread_create(&th[i], NULL, print_hello, &ids[i]);
        if (rc != 0) {
            perror("pthread_create");
            free(ids);
            exit(1);
        }
    }

    // 2) Esperar todas as threads terminarem (sincronização)
    for (int i = 0; i < NUM_THREADS; i++) {
        // pthread_join bloqueia até a thread th[i] terminar
        int rc = pthread_join(th[i], NULL);
        if (rc != 0) {
            perror("pthread_join");
            free(ids);
            exit(1);
        }
    }

    // 3) Limpeza e fim do processo
    free(ids);
    printf("Todas as threads finalizaram.\n");
    return 0;
}