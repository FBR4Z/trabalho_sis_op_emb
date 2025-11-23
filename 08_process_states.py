"""
process_states.py
Demonstra processos em diferentes estados (R, S, Z, T) no Linux.

Execução:
    python3 process_states.py

Em outro terminal, observe:
    ps -o pid,stat,comm -p <pid1>,<pid2>,<pid3>,<pid4>
ou
    top -H -p <PID_DO_PAI>
"""

import os
import time
import signal

print(f"PID do programa principal: {os.getpid()}")

# --------------------------------
# 1. Processo em RUNNING (R)
# --------------------------------
pidR = os.fork()
if pidR == 0:  # filho R
    while True:
        pass  # laço infinito -> sempre R (rodando ou pronto para rodar)

# --------------------------------
# 2. Processo em SLEEPING (S)
# --------------------------------
pidS = os.fork()
if pidS == 0:  # filho S
    while True:
        time.sleep(10)  # bloqueia em sleep -> estado "S" (esperando)

# --------------------------------
# 3. Processo em ZOMBIE (Z)
# --------------------------------
pidZ = os.fork()
if pidZ == 0:  # filho Z
    print(f"Filho Z (vai virar zumbi) PID={os.getpid()}")
    os._exit(0)  # termina imediatamente
else:
    time.sleep(1)  # pai NÃO chama wait() -> filho fica como zumbi

# --------------------------------
# 4. Processo em STOPPED (T)
# --------------------------------
pidT = os.fork()
if pidT == 0:  # filho T
    while True:
        signal.pause()  # espera sinal (fica bloqueado)
else:
    time.sleep(1)  # espera o filho iniciar
    os.kill(pidT, signal.SIGSTOP)  # envia SIGSTOP -> estado T (parado)

# --------------------------------
# Mantém o pai vivo para observação
# --------------------------------
print(f"Processos criados: R={pidR}  S={pidS}  Z={pidZ}  T={pidT}")
print("Use 'ps -o pid,stat,comm -p {0},{1},{2},{3}' para ver os estados.".format(
    pidR, pidS, pidZ, pidT))

while True:
    time.sleep(30)  # mantém processo pai vivo