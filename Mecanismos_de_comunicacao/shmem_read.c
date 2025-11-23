/*
 * LEITOR - Lê valores da memória compartilhada
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

int main(void) {
    int fd, value, *ptr;

    // Abre a memória compartilhada (já deve existir)
    fd = shm_open("/sharedmem", O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("shm_open - Execute o ESCRITOR primeiro!");
        exit(1);
    }

    // Mapeia a memória
    ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    printf("=== LEITOR: Lendo da memória compartilhada ===\n");
    printf("PID: %d\n\n", getpid());

    // Loop: lê valores
    for (;;) {
        value = *ptr;  // ← LÊ da memória compartilhada
        
        printf("[LEITOR PID %d] LEU: %d\n", getpid(), value);
        sleep(1);  // lê a cada 1 segundo
    }

    return 0;
}