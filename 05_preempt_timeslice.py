# preempt_timeslice.py
import threading
import time

c1 = 0
c2 = 0
running = True

def t1():
    global c1
    while running:
        c1 += 1

def t2():
    global c2
    while running:
        c2 += 1

# cria duas threads "gananciosas"
th1 = threading.Thread(target=t1)
th2 = threading.Thread(target=t2)

th1.start()
th2.start()

# imprime valores a cada 0.3s
for i in range(10):
    time.sleep(0.3)
    print(f"t1={c1}  t2={c2}")

running = False
th1.join()
th2.join()