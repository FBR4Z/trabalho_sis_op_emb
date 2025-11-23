// ============================================================================
// mutex_demo.c
// Demonstra exclusão mútua com pthreads (mutex) e compara com "race condition".
// Mostra: init/destroy, lock/unlock, trylock e uso de atributos "ERRORCHECK".
// ----------------------------------------------------------------------------
// Compilar:   gcc -O2 -pthread mutex_demo.c -o mutex_demo
// Executar:   ./mutex_demo
//             ./mutex_demo race        (para ver condição de corrida)
//             ./mutex_demo trylock     (para ver trylock em ação)
// ============================================================================

#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// ----------------------------------------------------------------------------
// Configuração do experimento
// ----------------------------------------------------------------------------
static const int N_ITERS = 500000;  // iterações por thread (de propósito grande)
static volatile long long contador = 0; // DADO COMPARTILHADO (vai provocar corrida)

static pthread_mutex_t mtx;   // nosso mutex
static int use_mutex = 1;     // liga/desliga proteção
static int demo_trylock = 0;  // demonstração de trylock

// ----------------------------------------------------------------------------
// Utilitário: cria mutex com atributo ERRORCHECK (ajuda a achar bugs didáticos)
// ----------------------------------------------------------------------------
static void criar_mutex_errorcheck(pthread_mutex_t *m) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);

    // ERRORCHECK: o sistema retorna erro se:
    //  - a thread der unlock em mutex que não possui mutex
    //  - der lock duas vezes no mesmo mutex (deadlock detectável)
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

    int rc = pthread_mutex_init(m, &attr);
    pthread_mutexattr_destroy(&attr);
    if (rc != 0) {
        fprintf(stderr, "pthread_mutex_init ERRORCHECK falhou: %s\n", strerror(rc));
        exit(1);
    }
}

// ----------------------------------------------------------------------------
// Região crítica simulada: incrementa o contador compartilhado
// ----------------------------------------------------------------------------
static inline void trabalho_na_secao_critica(void) {
    long long tmp = contador;  // lê
    // pequeno atraso para "forçar" interleaving das threads
    if ((tmp & 0x3FF) == 0) { sched_yield(); }
    tmp = tmp + 1;            // calcula
    contador = tmp;           // escreve  (não atômico!)
}

// ----------------------------------------------------------------------------
// Worker: cada thread incrementa N_ITERS vezes o contador
// ----------------------------------------------------------------------------
static void* worker(void* arg) {
    (void)arg;

    for (int i = 0; i < N_ITERS; i++) {
        if (use_mutex && !demo_trylock) {
            // Caminho comum: lock bloqueante (espera até conseguir)
            int rc = pthread_mutex_lock(&mtx);
            if (rc != 0) {
                fprintf(stderr, "lock falhou: %s\n", strerror(rc));
                exit(1);
            }
            trabalho_na_secao_critica();
            rc = pthread_mutex_unlock(&mtx);
            if (rc != 0) {
                fprintf(stderr, "unlock falhou: %s\n", strerror(rc));
                exit(1);
            }
        } else if (demo_trylock) {
            // Demonstração do trylock: tenta pegar; se ocupado, faz "plano B"
            int rc = pthread_mutex_trylock(&mtx);
            if (rc == 0) {
                // conseguiu: entra na seção crítica
                trabalho_na_secao_critica();
                pthread_mutex_unlock(&mtx);
            } else {
                // não conseguiu: faz outra coisa útil (aqui só cede a CPU)
                sched_yield();
            }
        } else {
            // Sem proteção: demonstra race condition
            trabalho_na_secao_critica();
        }
    }
    return NULL;
}

// ----------------------------------------------------------------------------
// Programa principal
// ----------------------------------------------------------------------------
int main(int argc, char** argv) {
    if (argc > 1) {
        if (strcmp(argv[1], "race") == 0) {
            use_mutex = 0;
            demo_trylock = 0;
        } else if (strcmp(argv[1], "trylock") == 0) {
            use_mutex = 1;
            demo_trylock = 1;
        }
    }

    printf("=== DEMO MUTEX (pthread) ===\n");
    printf("Modo: %s\n", (use_mutex ? (demo_trylock ? "trylock (não bloqueante)" : "lock/unlock (bloqueante)") : "RACE (sem mutex)"));
    printf("Threads: 2 | Iterações por thread: %d\n\n", N_ITERS);

    // Inicializa o mutex:
    //   - Poderíamos usar pthread_mutex_init(&mtx, NULL) para o tipo "NORMAL".
    //   - Aqui usamos ERRORCHECK para fins didáticos.
    criar_mutex_errorcheck(&mtx);

    pthread_t a, b;
    contador = 0;

    pthread_create(&a, NULL, worker, NULL);
    pthread_create(&b, NULL, worker, NULL);

    pthread_join(a, NULL);
    pthread_join(b, NULL);

    // Valor "correto" esperado: 2 * N_ITERS
    long long esperado = 2LL * N_ITERS;

    printf("\nContador FINAL = %lld | Esperado = %lld\n", contador, esperado);
    if (contador == esperado) {
        printf("✅ Resultado CORRETO (exclusão mútua garantida).\n");
    } else {
        printf("⚠️  Resultado INCORRETO (race condition ocorreu).\n");
    }

    // Destroi o mutex (libera recursos do SO)
    int rc = pthread_mutex_destroy(&mtx);
    if (rc != 0) {
        // Com ERRORCHECK, destruir um mutex ainda bloqueado retornaria erro.
        fprintf(stderr, "pthread_mutex_destroy: %s\n", strerror(rc));
    }

    return 0;
}
