#include <stdio.h>
#include <unistd.h>   // para usleep() em ambiente POSIX
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"   // necessário para uso de semáforos

/* =======================================================================
 * Exemplo 02 – Sincronização com Semáforo Binário
 * Demonstra como duas tasks podem se sincronizar usando um semáforo.
 * TaskProdutora dá (xSemaphoreGive) o semáforo.
 * TaskConsumidora pega (xSemaphoreTake) o semáforo antes de executar.
 * ======================================================================= */

SemaphoreHandle_t xSemaforo = NULL;

/* ---------- Task Produtora ---------- */
void vTaskProdutora(void *pvParameters)
{
    (void) pvParameters;
    for (;;)
    {
        printf("[Produtora] Fazendo trabalho e liberando semáforo...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));   // simula algum processamento

        /* Libera o semáforo — sinaliza para a consumidora */
        xSemaphoreGive(xSemaforo);
    }
}

/* ---------- Task Consumidora ---------- */
void vTaskConsumidora(void *pvParameters)
{
    (void) pvParameters;
    for (;;)
    {
        /* Espera o semáforo ser liberado pela produtora */
        if (xSemaphoreTake(xSemaforo, portMAX_DELAY) == pdTRUE)
        {
            printf("  [Consumidora] Recebeu sinal! Executando ação...\n");
            vTaskDelay(pdMS_TO_TICKS(500));
        }
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
    printf("=== FreeRTOS: Exemplo 02 – Semáforo Binário ===\n");

    /* Cria o semáforo binário */
    xSemaforo = xSemaphoreCreateBinary();

    if (xSemaforo == NULL)
    {
        printf("Falha ao criar semáforo!\n");
        return -1;
    }

    /* Cria as tasks de Produtora e Consumidora */
    xTaskCreate(vTaskProdutora, "Produtora", 1024, NULL, 2, NULL);
    xTaskCreate(vTaskConsumidora, "Consumidora", 1024, NULL, 1, NULL);

    /* Inicia o escalonador */
    vTaskStartScheduler();

    for (;;);  // se retornar, algo deu errado
}
