# rt_starvation.py
# sudo taskset -c 0 python3 rt_starvation.py
# O filho em tempo real monopoliza a CPU; 
# o pai quase não imprime (ou só de vez em quando,
#  por causa do RT throttling do kernel).

import os, time

pid = os.fork()

if pid == 0:
    # FILHO: tenta mudar para SCHED_FIFO
    param = os.sched_param(80)  # prioridade 1..99
    try:
        os.sched_setscheduler(0, os.SCHED_FIFO, param)
        while True:
            # ocupa CPU e imprime periodicamente
            for _ in range(50_000_000):
                pass
            os.write(1, b"FILHO (RT) rodando!\n")
    except PermissionError:
        print("Precisa rodar com sudo para SCHED_FIFO")
        os._exit(1)

else:
    # PAI: processo normal (SCHED_OTHER)
    for i in range(10):
        time.sleep(0.5)
        print(f"PAI ainda vivo (i={i})")
    os.kill(pid, 9)
    os.wait()