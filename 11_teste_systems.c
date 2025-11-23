//
// gcc teste_system.c -o teste_system
// ./teste_system


#include <stdio.h>
#include <stdlib.h>   // system()
#include <sys/types.h>
#include <unistd.h>   // getpid()

int main(void)
{
    // O processo pai inicia aqui
    printf("PID do Pai: %d\n", getpid());

    // Chama outro programa usando system()
    // → Cria um processo FILHO para rodar "./teste"
    // → O Pai espera o Filho terminar antes de continuar
    system("./teste");

    // Depois que o filho terminar, o Pai continua sua execução normalmente
    printf("\n[PAI] Programa continuou apos a funcao system()\n");

    return 0;
}