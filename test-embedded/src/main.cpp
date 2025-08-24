#include "main.h"
#include "hal/hal.h"
#include "hal/gpio.h"
#include "utils/logger.h"

#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "tasks/tasks.h"
#endif

// Global variables
volatile uint32_t g_systemTick = 0;

// Function prototypes
void SystemInit(void);
void SystemClock_Config(void);
void GPIO_Init(void);
void Error_Handler(void);

#ifdef USE_FREERTOS
void StartDefaultTask(void *argument);
#endif

int main(void) {
    // Initialize the system
    SystemInit();

    // Configure the system clock
    SystemClock_Config();

    // Initialize GPIO
    GPIO_Init();

    // Initialize HAL
    HAL_Init();

    // Initialize logger
    Logger_Init();

    LOG_INFO("Starting test-embedded embedded application...");

#ifdef USE_FREERTOS
    // Create default task
    xTaskCreate(StartDefaultTask, "DefaultTask", 128, NULL, 1, NULL);

    // Start the scheduler
    vTaskStartScheduler();
#else
    // Main application loop
    while (1) {
        // Toggle LED
        GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

        // Delay
        HAL_Delay(1000);

        // Process any pending tasks
        // Add your application logic here
    }
#endif

    // Should never reach here
    Error_Handler();
    return 0;
}

void SystemInit(void) {
    // System initialization code
    // Configure flash prefetch, instruction and data caches
    // Configure the Vector Table location
}

void SystemClock_Config(void) {
    // System clock configuration
    // Configure the main PLL and system clocks
    LOG_INFO("System clock configured");
}

void GPIO_Init(void) {
    // GPIO initialization
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIO clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure LED pin
    GPIO_InitStruct.Pin = LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

    LOG_INFO("GPIO initialized");
}

void Error_Handler(void) {
    // Error handling
    LOG_ERROR("System error occurred!");

    // Disable interrupts
    __disable_irq();

    // Infinite loop
    while (1) {
        // Blink LED rapidly to indicate error
        GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        for (volatile int i = 0; i < 100000; i++);
    }
}

#ifdef USE_FREERTOS
void StartDefaultTask(void *argument) {
    // Default task implementation
    for (;;) {
        // Toggle LED
        GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

        // Task delay
        vTaskDelay(pdMS_TO_TICKS(1000));

        LOG_INFO("Default task running...");
    }
}
#endif

// System tick handler
extern "C" void SysTick_Handler(void) {
    g_systemTick++;

#ifdef USE_FREERTOS
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
#endif
}
