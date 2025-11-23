# Caminho para o kernel FreeRTOS
FREERTOS_DIR = ../../FreeRTOS/FreeRTOS


# Nome do executável
TARGET = build/meu_exemplo5_timer

# Compilador e flags
CC = gcc
CFLAGS = -Wall -O2 -ggdb3 -DprojCOVERAGE_TEST=0 -DprojENABLE_TRACING=0
LDFLAGS = -lpthread

# Diretórios de include
INCLUDES = \
    -I. \
    -I$(FREERTOS_DIR)/Source/include \
    -I$(FREERTOS_DIR)/Source/portable/ThirdParty/GCC/Posix \
    -I$(FREERTOS_DIR)/Source/portable/ThirdParty/GCC/Posix/utils

# Fontes do FreeRTOS
SRC = \
    main.c \
    $(FREERTOS_DIR)/Source/list.c \
    $(FREERTOS_DIR)/Source/queue.c \
    $(FREERTOS_DIR)/Source/tasks.c \
    $(FREERTOS_DIR)/Source/timers.c \
    $(FREERTOS_DIR)/Source/event_groups.c \
    $(FREERTOS_DIR)/Source/portable/ThirdParty/GCC/Posix/port.c \
	$(FREERTOS_DIR)/Source/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c \
    $(FREERTOS_DIR)/Source/portable/MemMang/heap_3.c

# Regras
all:
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) $(INCLUDES) $(LDFLAGS) -o $(TARGET)

clean:
	rm -rf build
