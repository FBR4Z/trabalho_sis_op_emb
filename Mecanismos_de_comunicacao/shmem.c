/*
 * =========================================================================
 * EXEMPLO: Memória Compartilhada POSIX (shm_open + mmap)
 * =========================================================================
 * Objetivo:
 *   Mostrar como criar, mapear, escrever e ler em uma região de memória
 *   compartilhada POSIX — permitindo que múltiplos processos compartilhem
 *   valores sem usar pipes ou filas de mensagem.
 *
 * Bibliotecas utilizadas:
 *   shm_open()  → cria/abre memória compartilhada
 *   ftruncate() → define tamanho do segmento
 *   mmap()      → mapeia no espaço de endereçamento do processo
 *
 * Compilar:
 *   gcc shmem.c -o shmem_write -lrt
 *   gcc shmem.c -o shmem_read -lrt
 * Executar:
 *   ./shmem_write
 *   ./shmem_read
 * Observação:
 *   Execute dois terminais rodando este mesmo programa em paralelo
 *   para ver processos lendo/escrevendo a mesma área de memória.
 * =========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>      // O_CREAT, O_RDWR
#include <sys/stat.h>   // permissões
#include <sys/mman.h>   // mmap, shm_open
#include <unistd.h>     // ftruncate, sleep

int main(int argc, char *argv[]) {

    int fd;             // descritor da memória compartilhada
    int value;          // valor a ser lido/escrito
    int *ptr;           // ponteiro para a área mapeada

    /* =============================================================
     * PASSO 1 — Criar/Abrir a memória compartilhada
     * =============================================================
     * shm_open(nome, flags, permissões);
     *
     * Nome deve começar com "/"
     * Flags:
     *    O_CREAT → cria caso não exista
     *    O_RDWR  → abre para leitura e escrita
     */
    fd = shm_open("/sharedmem", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    /* =============================================================
     * PASSO 2 — Ajustar o tamanho do segmento
     * =============================================================
     * ftruncate(fd, tamanho)
     * Aqui, queremos armazenar APENAS 1 inteiro.
     */
    if (ftruncate(fd, sizeof(value)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    /* =============================================================
     * PASSO 3 — Mapear a memória para este processo
     * =============================================================
     * mmap(endereco, tamanho, proteção, flags, fd, offset)
     *
     * MAP_SHARED → alterações são visíveis para outros processos
     */
    ptr = mmap(NULL,
               sizeof(value),
               PROT_READ | PROT_WRITE,
               MAP_SHARED,
               fd,
               0);

    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    printf("=== Memória Compartilhada POSIX: /sharedmem ===\n");

    /* =============================================================
     * LOOP — Escreve e depois lê o valor compartilhado
     * =============================================================
     * Use dois terminais rodando este programa simultaneamente:
     *   Terminal 1: escreve e lê
     *   Terminal 2: escreve e lê
     * Observe como ambos compartilham o mesmo valor.
     */
    for (;;) {

        // ────────────── ESCREVE NA ÁREA COMPARTILHADA ──────────────
        // value = random() % 1000;   // valor aleatório
        // *ptr = value;              // escreve no segmento

        // printf("[PID %d] WROTE value = %d\n", getpid(), value);
        // sleep(1);

        // ─────────────── LÊ DA ÁREA COMPARTILHADA ──────────────────
        value = *ptr;
        printf("[PID %d] READ  value = %d\n", getpid(), value);
        sleep(1);
    }

    return 0;
}
