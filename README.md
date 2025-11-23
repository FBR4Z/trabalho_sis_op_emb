# Trabalho de Sistemas Operacionais Embarcados

Repositório com exemplos práticos de conceitos de sistemas operacionais embarcados.

## 📚 Conteúdo

### FreeRTOS
- **Exemplo01_3Tasks**: Demonstração de criação e gerenciamento de 3 tasks com diferentes prioridades
- **Exemplo02_Semaforo**: Sincronização entre tasks usando semáforos binários
- **Exemplo03_Queue**: Comunicação entre tasks usando filas (queues)

### Mecanismos de Comunicação entre Processos (IPC)
- **Pipes**: Comunicação unidirecional entre processos pai e filho
- **Message Queues**: Filas de mensagens POSIX para comunicação entre processos
- **Shared Memory**: Memória compartilhada POSIX para compartilhamento de dados

### Exemplos de Conceitos de SO
- **01-11**: Diversos exemplos demonstrando:
  - Fork e criação de processos
  - Estados de processos (R, S, Z, T)
  - Threads POSIX
  - Sinais
  - Escalonamento (preempção, prioridades RT)
  - Supervisor de processos

## 🛠️ Compilação e Execução

### FreeRTOS
```bash
cd Exemplo01_3Tasks
make
./build/meu_exemplo1_3tasks
```

### Programas C simples
```bash
gcc programa.c -o programa
./programa
```

### Programas com POSIX RT (message queues, shared memory)
```bash
gcc programa.c -o programa -lrt
./programa
```

### Programas com threads
```bash
gcc programa.c -o programa -lpthread
./programa
```

## 📋 Requisitos

- GCC
- Make
- FreeRTOS Kernel (incluído como submódulo)
- Linux (WSL, Ubuntu, etc.)

## 👨‍💻 Autor

[Fábio Braz] - Trabalho da disciplina de Sistemas Operacionais Embarcados

## 📄 Licença

Este projeto é livre para uso educacional.
