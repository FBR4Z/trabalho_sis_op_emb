# fork_example.py
# executar python3 fork_example.py
import os

pid = os.fork()
if pid == 0:
    print("Sou o filho (pid=0 no fork).")
else:
    print("Sou o pai, criei o filho com PID:", pid)