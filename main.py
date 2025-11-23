import os
import sys

print("=" * 50)
print("Teste Python no WSL")
print("=" * 50)

# Informações do sistema
print(f"Sistema: {sys.platform}")
print(f"Python: {sys.version}")
print(f"Diretório atual: {os.getcwd()}")
print(f"PID do processo: {os.getpid()}")

# Listar arquivos da pasta
print("\nArquivos na pasta:")
for arquivo in os.listdir('.'):
    print(f"  - {arquivo}")

# Testar comandos do sistema
print("\nTestando comando do sistema:")
resultado = os.system("whoami")
print(f"Código de retorno: {resultado}")