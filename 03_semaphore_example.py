# semaphore_example.py
# Sincronização com semáforo
# Python tem threading.Semaphore.
# Executar com: python3 semaphore_example.py

import threading

# Cria um semáforo inicializado com 1.
# Isso significa que só 1 thread de cada vez pode entrar na "região crítica".
sem = threading.Semaphore(1)

def tarefa(name):
    # O "with sem:" em Python é um atalho (context manager).
    # Ele chama automaticamente:
    #    sem.acquire()   -> equivalente ao sem_wait (espera o recurso)
    #    ... executa a região crítica ...
    #    sem.release()   -> equivalente ao sem_post (libera o recurso)
    #
    # Assim, não precisamos escrever acquire() e release() manualmente.
    with sem:
        print(f"Thread {name} executando")

# Cria duas threads, que vão disputar o mesmo semáforo
t1 = threading.Thread(target=tarefa, args=("A",))
t2 = threading.Thread(target=tarefa, args=("B",))

# Inicia as threads
t1.start()
t2.start()

# Aguarda ambas terminarem
t1.join()
t2.join()
