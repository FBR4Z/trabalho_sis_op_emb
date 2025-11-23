# pipe_example.py
# Comunicação entre processos (pipe)
# Usamos os.pipe(), os.write() e os.read().
# Executar com python3 pipe_example.py

import os

r, w = os.pipe()

pid = os.fork()
if pid == 0:
    # filho escreve
    os.close(r)
    msg = b"Mensagem do filho!"
    os.write(w, msg)
    os.close(w)
    os._exit(0)
else:
    # pai lê
    os.close(w)
    buffer = os.read(r, 50)
    print("Pai recebeu:", buffer.decode())
    os.close(r)