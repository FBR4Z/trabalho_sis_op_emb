/*
 * ESCRITOR - Escreve valores na memória compartilhada
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

int main(void) {
    int fd, value, *ptr;

    // Abre/cria a memória compartilhada
    fd = shm_open("/sharedmem", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }

    // Define tamanho
    if (ftruncate(fd, sizeof(int)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // Mapeia a memória
    ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    printf("=== ESCRITOR: Escrevendo na memória compartilhada ===\n");
    printf("PID: %d\n\n", getpid());

    // Loop: escreve valores aleatórios
    for (;;) {
        value = random() % 1000;  // número entre 0-999
        *ptr = value;             // ← ESCREVE na memória compartilhada
        
        printf("[ESCRITOR PID %d] ESCREVEU: %d\n", getpid(), value);
        sleep(2);  // espera 2 segundos
    }

    return 0;
}