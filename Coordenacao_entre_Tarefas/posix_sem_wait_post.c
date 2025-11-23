// ============================================================================
//  posix_sem_wait_post.c
// ============================================================================
// Demonstração didática de um semáforo POSIX (tipo sem_t) utilizando:
//     sem_init()   → inicializa o semáforo
//     sem_wait()   → operação DOWN (P): tenta entrar na seção crítica
//     sem_post()   → operação UP (V): libera a seção crítica
//     sem_destroy()→ destrói o semáforo
//
// Neste exemplo, até CAP threads podem acessar a seção crítica ao mesmo tempo.
// As demais threads ficam BLOQUEADAS (sem consumir CPU) até que uma vaga
// seja liberada.
//
// -----------------------------------------------------------------------------
// Compilar (em Linux/WSL/Ubuntu):
//     gcc -O2 -pthread posix_sem_wait_post.c -o posix_sem_wait_post
//
// Executar:
//     ./posix_sem_wait_post
//
// Obs.: Em macOS, sem_init() pode não existir. Nesses casos usa-se semáforo
// "nomeado" com sem_open() / sem_close() / sem_unlink().
// -----------------------------------------------------------------------------

#define _POSIX_C_SOURCE 200809L 
// Essa diretiva serve para habilitar as funcionalidades POSIX específicas 
// (isto é, padrões definidos pela norma IEEE 1003.x) no compilador C.
// “Ative as definições e funções da API POSIX até a versão 200809L.”
#include <pthread.h>   // threads POSIX
#include <semaphore.h> // sem_t, sem_init, sem_wait, sem_post, sem_destroy
#include <stdio.h>     // printf
#include <stdlib.h>    // rand, srand
#include <time.h>      // nanosleep, time
#include <unistd.h>    // sleep
#include <stdarg.h>    // va_list, va_start, va_end (para logf)

// -----------------------------------------------------------------------------
// Configurações globais
// -----------------------------------------------------------------------------
#define NTHREADS 5     // número total de threads criadas
#define CAP      2     // capacidade do semáforo (quantas entram ao mesmo tempo)

// -----------------------------------------------------------------------------
// Variáveis globais
// -----------------------------------------------------------------------------
static sem_t sem;  // semáforo POSIX (não nomeado, "unnamed semaphore")
                   // CAP = número inicial de permissões disponíveis.
                   // static - Restringe a visibilidade da variável ou função 
                   // ao arquivo atual (não exporta símbolos).

static pthread_mutex_t io_mtx = PTHREAD_MUTEX_INITIALIZER;
// Mutex usado apenas para sincronizar os printf, evitando mistura de textos
// no terminal durante execuções paralelas.

// -----------------------------------------------------------------------------
// Função auxiliar msleep(ms)
// -----------------------------------------------------------------------------
static void msleep(int ms) { 
    // Essa função é interna ao arquivo — não pode ser chamadas de outro .c
    // Suspende a thread atual por 'ms' milissegundos (simulação de trabalho).
    // Essa função é interna ao arquivo — não pode ser chamadas de outro .c
    struct timespec ts = {
        .tv_sec  = ms / 1000,
        .tv_nsec = (ms % 1000) * 1000000L
    };
    nanosleep(&ts, NULL);
}

// -----------------------------------------------------------------------------
// Função auxiliar logf() — printf thread-safe
// -----------------------------------------------------------------------------
static void logf(const char *fmt, ...) {
    // Essa função é interna ao arquivo — não pode ser chamadas de outro .c
    // const é usado para indicar que o valor não pode ser alterado após ser definido.
    // O valor da variável é imutável (somente leitura).
    // const na função indica que a função não deve alterar o valor passado (por segurança e clareza).

    // Em C, o ... (três pontos) indica que a função aceita um número variável de argumentos.

    va_list ap; // Mecanismo que permite “varrer” os argumentos variáveis (definido em <stdarg.h>).
                // Cria um objeto para armazenar a lista de argumentos.
    va_start(ap, fmt); // Inicializa a varredura, dizendo que os argumentos extras vêm depois de fmt.

    pthread_mutex_lock(&io_mtx);   // evita interferência entre prints
    vprintf(fmt, ap);              // imprime com formatação variável
                                    // Variante de printf que usa uma va_list em vez de argumentos diretos.

    fflush(stdout);
    pthread_mutex_unlock(&io_mtx);
    va_end(ap); // Finaliza a varredura dos argumentos.
}

// -----------------------------------------------------------------------------
// Função executada por cada thread (worker)
// -----------------------------------------------------------------------------
void* worker(void* arg) {
    long id = (long)arg;  // identificador numérico da thread

    // Cada thread tenta entrar 3 vezes na seção crítica
    for (int i = 0; i < 3; i++) {
        logf("[W%ld] quer entrar (sem_wait)\n", id);

        // ============================================================
        // sem_wait(&sem)
        // ------------------------------------------------------------
        // Opera como o "DOWN(s)" do pseudocódigo clássico:
        //    s.counter ← s.counter - 1
        //    if (s.counter < 0) then
        //        bloqueia a tarefa e a coloca na fila de espera
        //
        // Quando o semáforo = 0 → thread fica BLOQUEADA automaticamente.
        // ============================================================
        sem_wait(&sem);

        // Se chegou até aqui, a thread obteve uma "vaga" (recurso disponível).
        logf("  [W%ld] ENTROU na seção crítica\n", id);

        // Simula algum trabalho na seção crítica (uso do recurso).
        msleep(400 + (rand() % 300));

        logf("  [W%ld] saindo (sem_post)\n", id);

        // ============================================================
        // sem_post(&sem)
        // ------------------------------------------------------------
        // Opera como o "UP(s)" do pseudocódigo clássico:
        //    s.counter ← s.counter + 1
        //    if (s.counter <= 0) then
        //        acorda uma tarefa bloqueada (se houver)
        //
        // Libera o recurso, permitindo que outra thread entre.
        // ============================================================
        sem_post(&sem);

        // Simula trabalho fora da seção crítica (não protegido pelo semáforo)
        msleep(200 + (rand() % 300));
    }

    return NULL;
}

// -----------------------------------------------------------------------------
// Função principal
// -----------------------------------------------------------------------------
int main(void) {
    srand((unsigned)time(NULL)); // inicializa gerador de números aleatórios

    // Inicializa o semáforo:
    // sem_init(semáforo, compartilhamento, valor_inicial)
    // pshared = 0 → semáforo compartilhado APENAS entre threads do MESMO processo
    // valor_inicial = CAP → número de recursos disponíveis no início
    if (sem_init(&sem, 0, CAP) != 0) {
        perror("sem_init");
        return 1;
    }

    printf("\n=== POSIX Semáforo (sem_t) ===\n");
    printf("Capacidade inicial = %d | Threads = %d\n\n", CAP, NTHREADS);

    // Cria NTHREADS threads, cada uma executando a função worker()
    pthread_t th[NTHREADS];
    for (long i = 0; i < NTHREADS; i++) {
        pthread_create(&th[i], NULL, worker, (void*)i);
    }

    // Aguarda o término de todas as threads
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(th[i], NULL);
    }

    // Destrói o semáforo (libera recursos do kernel)
    sem_destroy(&sem);

    puts("\nTodas as threads terminaram. Fim da simulação.\n");
    return 0;
}
