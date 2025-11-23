
// Demonstra multitarefa através do uso do fork e o gerenciamento de processos
// gcc fork_execve.c -o fork_execve
// ./fork_execve



#include <unistd.h>    // fork(), execve()
#include <sys/types.h> // tipos pid_t etc.
#include <sys/wait.h>  // wait()
#include <stdio.h>     // printf(), perror()
#include <stdlib.h>    // exit()

int main(int argc, char *argv[], char *envp[])
{
    int pid;                  // variável que vai guardar o PID do processo filho

    pid = fork();             // cria um novo processo (copia do atual)
                              // retorna:
                              // < 0 → erro
                              // = 0 → estamos no processo FILHO
                              // > 0 → estamos no processo PAI (retorna o PID do filho)

    if (pid < 0)              // caso o fork falhe
    {
        perror("Erro: ");     // imprime mensagem de erro
        exit(-1);             // encerra o processo com código -1
    }
    else                      // fork funcionou
        if (pid > 0)          // estamos no processo PAI?
            wait(0);          // sim → espera o filho terminar antes de continuar
        else                  // senão, estamos no processo FILHO
        {
            // substitui o código do filho por outro programa
            // aqui o binário /bin/date (com parâmetros da linha de comando)
            execve("/bin/date", argv, envp);

            // se execve retornar, significa que houve erro
            perror("Erro: "); 
        }

    // este trecho é executado apenas pelo pai (depois do filho terminar)
    // ou pelo filho se execve falhar
    printf("Tchau !\n");      

    exit(0);                  // encerra o processo normalmente
}