#include <stdio.h>
#include <unistd.h>   // usleep() no port POSIX

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"   // necessário para Software Timers

/* =======================================================================
 * Exemplo 05 – Uso de Software Timer no FreeRTOS
 * Demonstra como criar timers periódicos e de disparo único.
 * ======================================================================= */

/* Handles dos timers */
TimerHandle_t xTimerBlink;
TimerHandle_t xTimerOneShot;

/* ---------- Callback do timer periódico ---------- */
void vTimerBlinkCallback(TimerHandle_t xTimer)
{
    static int estado = 0;
    estado = !estado;
    printf("[TimerBlink] LED virtual: %s\n", estado ? "ON" : "OFF");
}

/* ---------- Callback do timer de disparo único ---------- */
void vTimerOneShotCallback(TimerHandle_t xTimer)
{
    printf("[TimerOneShot] Ação executada uma única vez após 5 s!\n");
}

/* =======================================================================
 * Hooks obrigatórios (mínimos para POSIX)
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

void vApplicationDaemonTaskStartupHook(void)
{
    printf("[DaemonTask] Serviço de timers iniciado.\n");
}

/* =======================================================================
 * Ponto de entrada
 * ======================================================================= */
int main(void)
{
    printf("=== FreeRTOS: Exemplo 05 – Software Timer ===\n");

    /* Cria um timer periódico (LED piscando a cada 1 s) */
    xTimerBlink = xTimerCreate("TimerBlink",
                               pdMS_TO_TICKS(1000),   // período: 1 s
                               pdTRUE,                // pdTRUE → periódico
                               NULL,
                               vTimerBlinkCallback);

    /* Cria um timer de disparo único (executa após 5 s) */
    xTimerOneShot = xTimerCreate("TimerOneShot",
                                 pdMS_TO_TICKS(5000), // atraso: 5 s
                                 pdFALSE,              // pdFALSE → uma vez só
                                 NULL,
                                 vTimerOneShotCallback);

    if (xTimerBlink == NULL || xTimerOneShot == NULL)
    {
        printf("Falha ao criar timers!\n");
        return -1;
    }

    /* Inicia ambos os timers */
    xTimerStart(xTimerBlink, 0);
    xTimerStart(xTimerOneShot, 0);

    /* Inicia o escalonador */
    vTaskStartScheduler();

    for (;;); // nunca deve chegar aqui
}
