/*
 * Demonstração: Tempo real (SCHED_FIFO) e prioridade sobre tarefa "normal"
 *
 * Ideia:
 *  - Criamos um processo FILHO que muda sua política de escalonamento para SCHED_FIFO
 *    (tempo real) com prioridade alta e entra em busy-loop. De tempos em tempos ele
 *    imprime uma mensagem.
 *  - O processo PAI fica em SCHED_OTHER (CFS) e tenta imprimir a cada 500 ms.
 *  - Quando os dois disputam o MESMO core, o FILHO tende a ocupar a CPU quase o tempo todo,
 *    e o PAI só aparece ocasionalmente (quando sobra tempo ou por throttling de RT).
 *
 * Como compilar:
 *   gcc rt_starvation.c -o rt_starvation
 *
 * Como executar (importante rodar os dois no MESMO core):
 *   sudo taskset -c 0 ./rt_starvation
 *
 * Observação:
 *   - É preciso sudo (ou CAP_SYS_NICE) para elevar a tarefa a SCHED_FIFO.
 *   - O kernel Linux costuma ter "RT throttling" (limita uso de CPU por tarefas RT),
 *     o que pode permitir que o PAI ainda imprima um pouco. Para DEMONSTRAÇÃO,
 *     você pode desativar temporariamente: sudo sysctl -w kernel.sched_rt_runtime_us=-1
 *     e depois restaurar (por ex.): sudo sysctl -w kernel.sched_rt_runtime_us=950000
 */

#define _GNU_SOURCE           // habilita APIs GNU como sched_setscheduler
#include <stdio.h>           // printf, perror
#include <unistd.h>          // fork, write, nanosleep
#include <sched.h>           // sched_setscheduler, struct sched_param, SCHED_FIFO
#include <stdlib.h>          // exit, _exit
#include <errno.h>           // errno
#include <sys/wait.h>        // wait, waitpid
#include <signal.h>          // kill
#include <time.h>            // nanosleep
#include <string.h>          // strerror

int main(void) {
    pid_t pid = fork();      // cria um processo filho

    if (pid < 0) {
        // Erro ao criar processo
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // ================================
        // FILHO: Política de tempo real
        // ================================
        // Define prioridade alta (intervalo válido: 1..99 para SCHED_FIFO/SCHED_RR).
        struct sched_param sp = { .sched_priority = 80 };

        // Tenta trocar a política de escalonamento do processo atual (pid 0 = self).
        if (sched_setscheduler(0, SCHED_FIFO, &sp) != 0) {
            // Se falhar, provavelmente falta permissão (sudo/CAP_SYS_NICE).
            fprintf(stderr, "sched_setscheduler falhou: %s\n", strerror(errno));
            _exit(1);
        }

        // Texto fixo a ser impresso pelo FILHO em intervalos de iteração.
        static const char msg[] = "FILHO (SCHED_FIFO) rodando: prioridade sobre o PAI\n";

        // Busy-loop contínuo: não fazemos sleep nem yield, para não "ceder" a CPU.
        // A cada N iterações, escrevemos uma mensagem (write em STDOUT evita buffering).
        volatile unsigned long long x = 0;
        const unsigned long long N = 80ULL * 1000ULL * 1000ULL; // ajuste a frequência

        for (;;) {
            x++;
            if ((x % N) == 0) {
                // write() é async-signal-safe e não depende de buffering da stdio.
                ssize_t _unused = write(STDOUT_FILENO, msg, sizeof(msg) - 1);
                (void)_unused; // evita warning de variável não usada
            }
        }

        // (nunca chega aqui)
        _exit(0);

    } else {
        // ================================
        // PAI: Tarefa normal (SCHED_OTHER/CFS)
        // ================================
        // O pai apenas imprime "batimentos" a cada 500 ms.
        // Se o FILHO (RT) estiver pegando a CPU continuamente no MESMO core,
        // essas mensagens do pai tendem a atrasar/aparecer menos.
        for (int i = 0; i < 10; i++) {
            struct timespec ts = { .tv_sec = 0, .tv_nsec = 500 * 1000 * 1000 }; // 500 ms
            nanosleep(&ts, NULL);  // pausa o PAI; o FILHO RT segue rodando

            printf("PAI (SCHED_OTHER) ainda vivo (i=%d)\n", i);
            fflush(stdout);        // força saída imediata (stdout é bufferizado)
        }

        // Encerra o filho ao final da demonstração, para não ficar consumindo CPU.
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
        return 0;
    }
}