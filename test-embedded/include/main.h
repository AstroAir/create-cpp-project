#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

// System includes
#include <stdint.h>
#include <stdbool.h>

// HAL includes
#include "hal/hal.h"

// Configuration
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC

// Function prototypes
void SystemInit(void);
void SystemClock_Config(void);
void GPIO_Init(void);
void Error_Handler(void);

// Global variables
extern volatile uint32_t g_systemTick;

#ifdef __cplusplus
}
#endif

#endif // MAIN_H
