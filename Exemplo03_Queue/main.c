#include <stdio.h>
#include <unistd.h>   // usleep() em ambiente POSIX

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"    // necessário para usar filas (queues)

/* =======================================================================
 * Exemplo 03 – Comunicação entre Tasks via Fila (Queue)
 * Demonstra o envio e recebimento de dados entre tarefas de forma segura.
 * Uma tarefa envia inteiros para uma fila; outra lê e imprime.
 * ======================================================================= */

QueueHandle_t xFila = NULL;

/* ---------- Task Produtora ---------- */
void vTaskProdutora(void *pvParameters)
{
    (void) pvParameters;
    int contador = 0;

    for (;;)
    {
        /* Envia um valor incremental para a fila */
        contador++;
        if (xQueueSend(xFila, &contador, portMAX_DELAY) == pdPASS)
        {
            printf("[Produtora] Enviou valor %d para a fila.\n", contador);
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // simula trabalho
    }
}

/* ---------- Task Consumidora ---------- */
void vTaskConsumidora(void *pvParameters)
{
    (void) pvParameters;
    int recebido;

    for (;;)
    {
        /* Espera receber um valor da fila (bloqueia até chegar algo) */
        if (xQueueReceive(xFila, &recebido, portMAX_DELAY) == pdTRUE)
        {
            printf("  [Consumidora] Recebeu valor %d da fila!\n", recebido);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* =======================================================================
 * Implementações obrigatórias (hooks e suporte estático)
 * ======================================================================= */

void vApplicationMallocFailedHook(void)
{
    printf("ERRO: malloc falhou!\n");
    fflush(stdout);
    taskDISABLE_INTERRUPTS();
    for(;;);
}

void vAssertCalled(const char * const pcFileName, unsigned long ulLine)
{
    printf("ASSERT falhou em %s:%lu\n", pcFileName, ulLine);
    fflush(stdout);
    taskDISABLE_INTERRUPTS();
    for(;;);
}

void vApplicationTickHook(void) { }
void vApplicationIdleHook(void) { usleep(1000); }

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vApplicationDaemonTaskStartupHook(void) { }

/* =======================================================================
 * Ponto de entrada
 * ======================================================================= */
int main(void)
{
    printf("=== FreeRTOS: Exemplo 03 – Comunicação com Fila ===\n");

    /* Cria uma fila com 5 elementos do tipo int */
    xFila = xQueueCreate(5, sizeof(int));

    if (xFila == NULL)
    {
        printf("Falha ao criar fila!\n");
        return -1;
    }

    /* Cria as tasks */
    xTaskCreate(vTaskProdutora, "Produtora", 1024, NULL, 2, NULL);
    xTaskCreate(vTaskConsumidora, "Consumidora", 1024, NULL, 1, NULL);

    /* Inicia o escalonador */
    vTaskStartScheduler();

    for (;;);  // se retornar, erro crítico
}
