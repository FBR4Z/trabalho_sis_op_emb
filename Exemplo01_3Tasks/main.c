#include <stdio.h>
#include <unistd.h> // para usleep() (apenas para exemplo em POSIX)
#include "FreeRTOS.h"
#include "task.h"

/* ---------- Task 1 ---------- */
void vTaskA(void *pvParameters) {
    while (1) {
        printf("Task A executando...\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* ---------- Task 2 ---------- */
void vTaskB(void *pvParameters) {
    while (1) {
        printf("Task B executando...\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ---------- Task 3 ---------- */
void vTaskC(void *pvParameters) {
    while (1) {
        printf("Task C executando...\n");
        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}

/* ============================ */
/* Implementações obrigatórias  */
/* ============================ */

/* Chamado se o malloc falhar */
void vApplicationMallocFailedHook(void) {
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

/* Hook chamado a cada tick (opcional) */
void vApplicationTickHook(void) {
    // pode deixar vazio
}

/* Hook chamado quando o sistema está ocioso */
void vApplicationIdleHook(void) {
    // idle task
    usleep(1000); // evita consumir CPU demais
}

/* Hooks usados se o sistema estiver configurado para alocação estática */
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

/* Chamado ao iniciar o daemon de timers */
void vApplicationDaemonTaskStartupHook(void) {
    // pode deixar vazio também
}


/* ---------- Ponto de entrada ---------- */
int main(void) {
    printf("=== FreeRTOS: Exemplo com 3 Tasks ===\n");

    /* Cria 3 tasks com diferentes prioridades */
    xTaskCreate(vTaskA, "TaskA", 1024, NULL, 2, NULL);
    xTaskCreate(vTaskB, "TaskB", 1024, NULL, 1, NULL);
    xTaskCreate(vTaskC, "TaskC", 1024, NULL, 1, NULL);

    /* Inicia o escalonador */
    vTaskStartScheduler();

    /* Se chegar aqui, houve erro */
    for(;;);
}
