# signal_example.py
# Demonstração de tratamento de sinais em Python
# Ao rodar este programa, ele fica em loop infinito imprimindo uma mensagem.
# Se você pressionar Ctrl+C no terminal, o sinal SIGINT é enviado ao processo.
# Em vez de encerrar imediatamente, o programa captura o sinal e executa a função handler().

import signal   # módulo para trabalhar com sinais do sistema operacional
import time     # módulo para pausas (sleep)
import sys      # módulo do sistema, usado aqui para encerrar o programa (sys.exit)

# Função "handler" que será chamada quando o processo receber um sinal registrado
def handler(sig, frame):
    # sig: número do sinal recebido (ex.: 2 = SIGINT)
    # frame: informações sobre o estado atual da execução (não usamos aqui)
    print(f"Recebi sinal {sig}! Encerrando programa...")
    sys.exit(0)  # encerra o programa de forma controlada

# Associa o sinal SIGINT (gerado ao pressionar Ctrl+C) à função handler
signal.signal(signal.SIGINT, handler)

# Loop infinito simulando execução contínua
while True:
    print("Executando... pressione Ctrl+C")
    time.sleep(1)  # pausa de 1 segundo entre as mensagens