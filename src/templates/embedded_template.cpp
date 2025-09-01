#include "embedded_template.h"

#include <spdlog/fmt/fmt.h>

#include <iostream>

#include "../utils/file_utils.h"

using namespace utils;

EmbeddedTemplate::EmbeddedTemplate(const CliOptions& options) : TemplateBase(options) {}

bool EmbeddedTemplate::create() {
    std::cout << "🔧 Creating Embedded project: " << options_.projectName << "\n";

    if (!createProjectStructure()) {
        std::cerr << "❌ Failed to create project structure\n";
        return false;
    }

    if (!createBuildSystem()) {
        std::cerr << "❌ Failed to create build system\n";
        return false;
    }

    if (!setupPackageManager()) {
        std::cerr << "❌ Failed to setup package manager\n";
        return false;
    }

    if (!setupHardwareAbstraction()) {
        std::cerr << "❌ Failed to setup hardware abstraction layer\n";
        return false;
    }

    if (!setupTestFramework()) {
        std::cerr << "❌ Failed to setup test framework\n";
        return false;
    }

    if (!setupDebugging()) {
        std::cerr << "❌ Failed to setup debugging configuration\n";
        return false;
    }

    if (!initializeGit(options_.projectName)) {
        std::cerr << "❌ Failed to initialize Git repository\n";
        return false;
    }

    std::cout << "✅ Embedded project created successfully!\n";
    printUsageGuide();
    return true;
}

bool EmbeddedTemplate::createProjectStructure() {
    std::string projectPath = options_.projectName;

    // Create main directories
    std::vector<std::string> directories = {projectPath,
                                            FileUtils::combinePath(projectPath, "src"),
                                            FileUtils::combinePath(projectPath, "src/hal"),
                                            FileUtils::combinePath(projectPath, "src/drivers"),
                                            FileUtils::combinePath(projectPath, "src/tasks"),
                                            FileUtils::combinePath(projectPath, "src/utils"),
                                            FileUtils::combinePath(projectPath, "include"),
                                            FileUtils::combinePath(projectPath, "include/hal"),
                                            FileUtils::combinePath(projectPath, "include/drivers"),
                                            FileUtils::combinePath(projectPath, "include/tasks"),
                                            FileUtils::combinePath(projectPath, "include/utils"),
                                            FileUtils::combinePath(projectPath, "tests"),
                                            FileUtils::combinePath(projectPath, "tests/unit"),
                                            FileUtils::combinePath(projectPath, "tests/hardware"),
                                            FileUtils::combinePath(projectPath, "docs"),
                                            FileUtils::combinePath(projectPath, "scripts"),
                                            FileUtils::combinePath(projectPath, "config"),
                                            FileUtils::combinePath(projectPath, "linker"),
                                            FileUtils::combinePath(projectPath, "startup")};

    for (const auto& dir : directories) {
        if (!FileUtils::createDirectory(dir)) {
            std::cerr << "❌ Failed to create directory: " << dir << "\n";
            return false;
        }
    }

    // Create main application files
    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "src/main.cpp"),
                                getMainCppContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "include/main.h"),
                                getMainHeaderContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "README.md"),
                                getReadmeContent())) {
        return false;
    }

    std::cout << "📁 Project structure created\n";
    return true;
}

bool EmbeddedTemplate::createBuildSystem() {
    std::string projectPath = options_.projectName;
    std::string buildContent;

    if (usePlatformIO()) {
        // Create PlatformIO configuration
        buildContent = getPlatformIOContent();
        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "platformio.ini"),
                                    buildContent)) {
            return false;
        }
    } else if (useArduino()) {
        // Create Arduino sketch structure
        std::string sketchDir = FileUtils::combinePath(projectPath, options_.projectName);
        if (!FileUtils::createDirectory(sketchDir)) {
            return false;
        }
        buildContent = getArduinoContent();
        if (!FileUtils::writeToFile(
                    FileUtils::combinePath(sketchDir, options_.projectName + ".ino"),
                    buildContent)) {
            return false;
        }
    } else {
        // Use traditional Makefile or CMake
        switch (options_.buildSystem) {
            case BuildSystem::CMake:
                buildContent = getCMakeContent();
                if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "CMakeLists.txt"),
                                            buildContent)) {
                    return false;
                }
                break;
            case BuildSystem::Make:
            default:
                buildContent = getMakefileContent();
                if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "Makefile"),
                                            buildContent)) {
                    return false;
                }
                break;
        }
    }

    std::cout << "🔧 Build system configured\n";
    return true;
}

bool EmbeddedTemplate::setupPackageManager() {
    // Embedded projects typically don't use traditional package managers
    // But we can setup PlatformIO library management or Git submodules
    std::string projectPath = options_.projectName;

    if (usePlatformIO()) {
        // PlatformIO handles dependencies via platformio.ini
        std::cout << "📦 PlatformIO will manage dependencies\n";
    } else {
        // Create a libs directory for manual library management
        if (!FileUtils::createDirectory(FileUtils::combinePath(projectPath, "libs"))) {
            return false;
        }

        // Create a simple library management script
        std::string libScript = R"(#!/bin/bash
# Library management script for embedded project
# Add your libraries as git submodules or download them here

echo "Setting up embedded libraries..."
# Example: git submodule add https://github.com/FreeRTOS/FreeRTOS.git libs/FreeRTOS
)";

        if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "scripts/setup_libs.sh"),
                                    libScript)) {
            return false;
        }
    }

    std::cout << "📦 Package management configured\n";
    return true;
}

bool EmbeddedTemplate::setupHardwareAbstraction() {
    std::string projectPath = options_.projectName;

    // Create HAL files
    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "include/hal/hal.h"),
                                getHalHeaderContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "src/hal/hal.cpp"),
                                getHalCppContent())) {
        return false;
    }

    // Create GPIO abstraction
    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "include/hal/gpio.h"),
                                getGpioHeaderContent())) {
        return false;
    }

    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "src/hal/gpio.cpp"),
                                getGpioCppContent())) {
        return false;
    }

    std::cout << "🔌 Hardware abstraction layer created\n";
    return true;
}

bool EmbeddedTemplate::setupTestFramework() {
    if (!options_.includeTests) {
        return true;
    }

    std::string projectPath = options_.projectName;
    std::string testsPath = FileUtils::combinePath(projectPath, "tests");

    // For embedded, we typically use Unity test framework
    std::string testContent = getUnityTestContent();
    if (!FileUtils::writeToFile(FileUtils::combinePath(testsPath, "unit/test_main.cpp"),
                                testContent)) {
        return false;
    }

    // Hardware-in-the-loop tests
    std::string hwTestContent = getHardwareTestContent();
    if (!FileUtils::writeToFile(FileUtils::combinePath(testsPath, "hardware/test_gpio.cpp"),
                                hwTestContent)) {
        return false;
    }

    std::cout << "🧪 Test framework configured\n";
    return true;
}

bool EmbeddedTemplate::setupDebugging() {
    std::string projectPath = options_.projectName;

    // Create OpenOCD configuration
    std::string openocdConfig = getOpenOCDConfigContent();
    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "config/openocd.cfg"),
                                openocdConfig)) {
        return false;
    }

    // Create GDB initialization script
    std::string gdbInit = getGdbInitContent();
    if (!FileUtils::writeToFile(FileUtils::combinePath(projectPath, "config/gdbinit"), gdbInit)) {
        return false;
    }

    std::cout << "🐛 Debugging configuration created\n";
    return true;
}

// Helper methods
std::string EmbeddedTemplate::getTargetMCU() const {
    return "STM32F4";  // Default, can be made configurable
}

std::string EmbeddedTemplate::getTargetBoard() const {
    return "STM32F4-Discovery";  // Default, can be made configurable
}

std::string EmbeddedTemplate::getRTOSType() const {
    return "FreeRTOS";  // Default, can be made configurable
}

bool EmbeddedTemplate::useHAL() const {
    return true;  // Default to using HAL
}

bool EmbeddedTemplate::useFreeRTOS() const {
    return true;  // Default to using FreeRTOS
}

bool EmbeddedTemplate::useArduino() const {
    return false;  // Default to not using Arduino framework
}

bool EmbeddedTemplate::usePlatformIO() const {
    return false;  // Default to not using PlatformIO
}

std::string EmbeddedTemplate::getMainCppContent() {
    return fmt::format(R"(#include "main.h"
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

int main(void) {{
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

    LOG_INFO("Starting {} embedded application...");

#ifdef USE_FREERTOS
    // Create default task
    xTaskCreate(StartDefaultTask, "DefaultTask", 128, NULL, 1, NULL);

    // Start the scheduler
    vTaskStartScheduler();
#else
    // Main application loop
    while (1) {{
        // Toggle LED
        GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

        // Delay
        HAL_Delay(1000);

        // Process any pending tasks
        // Add your application logic here
    }}
#endif

    // Should never reach here
    Error_Handler();
    return 0;
}}

void SystemInit(void) {{
    // System initialization code
    // Configure flash prefetch, instruction and data caches
    // Configure the Vector Table location
}}

void SystemClock_Config(void) {{
    // System clock configuration
    // Configure the main PLL and system clocks
    LOG_INFO("System clock configured");
}}

void GPIO_Init(void) {{
    // GPIO initialization
    GPIO_InitTypeDef GPIO_InitStruct = {{0}};

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
}}

void Error_Handler(void) {{
    // Error handling
    LOG_ERROR("System error occurred!");

    // Disable interrupts
    __disable_irq();

    // Infinite loop
    while (1) {{
        // Blink LED rapidly to indicate error
        GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        for (volatile int i = 0; i < 100000; i++);
    }}
}}

#ifdef USE_FREERTOS
void StartDefaultTask(void *argument) {{
    // Default task implementation
    for (;;) {{
        // Toggle LED
        GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

        // Task delay
        vTaskDelay(pdMS_TO_TICKS(1000));

        LOG_INFO("Default task running...");
    }}
}}
#endif

// System tick handler
extern "C" void SysTick_Handler(void) {{
    g_systemTick++;

#ifdef USE_FREERTOS
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {{
        xPortSysTickHandler();
    }}
#endif
}}
)",
                       options_.projectName);
}

std::string EmbeddedTemplate::getMainHeaderContent() {
    return R"(#ifndef MAIN_H
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
)";
}

std::string EmbeddedTemplate::getReadmeContent() {
    return fmt::format(R"(# {}

An embedded C++ project for microcontrollers.

## Features

- Hardware Abstraction Layer (HAL)
- GPIO control
- UART communication
- SPI/I2C support
- FreeRTOS integration (optional)
- Debugging support with OpenOCD/GDB

## Hardware Requirements

- Target MCU: {}
- Development Board: {}
- Debugger: ST-Link or J-Link

## Building

### Prerequisites

- ARM GCC toolchain
- OpenOCD (for debugging)
- Make or CMake

### Build Steps

```bash
# Using Make
make clean
make all

# Using CMake
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake ..
make
```

## Flashing

```bash
# Using OpenOCD
openocd -f config/openocd.cfg -c "program build/{}.elf verify reset exit"

# Using ST-Link
st-flash write build/{}.bin 0x8000000
```

## Debugging

```bash
# Start OpenOCD server
openocd -f config/openocd.cfg

# In another terminal, start GDB
arm-none-eabi-gdb build/{}.elf
(gdb) target remote localhost:3333
(gdb) load
(gdb) continue
```

## Project Structure

- `src/` - Source files
- `include/` - Header files
- `src/hal/` - Hardware abstraction layer
- `src/drivers/` - Device drivers
- `src/tasks/` - FreeRTOS tasks (if enabled)
- `linker/` - Linker scripts
- `startup/` - Startup code
- `config/` - Configuration files

## Configuration

Edit the following files to configure your project:
- `include/main.h` - Main configuration
- `config/openocd.cfg` - OpenOCD configuration
- `linker/STM32F4xx_FLASH.ld` - Memory layout

## License

MIT License
)",
                       options_.projectName, getTargetMCU(), getTargetBoard(), options_.projectName,
                       options_.projectName, options_.projectName);
}

// Add placeholder implementations for missing methods
std::string EmbeddedTemplate::getPlatformIOContent() {
    return fmt::format(R"([env:{}]
platform = ststm32
board = nucleo_f401re
framework = arduino

monitor_speed = 115200
upload_protocol = stlink

build_flags =
    -DUSE_HAL_DRIVER
    -DSTM32F401xE

lib_deps =
    # Add your library dependencies here
)",
                       getTargetBoard());
}

std::string EmbeddedTemplate::getArduinoContent() {
    return fmt::format(R"(// {} Arduino Sketch
// Generated by CPP-Scaffold

void setup() {{
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize LED pin
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Starting {} application...");
}}

void loop() {{
  // Blink LED
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  Serial.println("Application running...");
}}
)",
                       options_.projectName, options_.projectName);
}

std::string EmbeddedTemplate::getCMakeContent() {
    return fmt::format(R"(cmake_minimum_required(VERSION 3.15)

# Set the toolchain file
set(CMAKE_TOOLCHAIN_FILE "${{CMAKE_CURRENT_SOURCE_DIR}}/cmake/arm-none-eabi.cmake")

project({} C CXX ASM)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MCU configuration
set(MCU_FAMILY STM32F4xx)
set(MCU_MODEL STM32F401xE)

# Compiler flags
set(CMAKE_C_FLAGS "${{CMAKE_C_FLAGS}} -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(CMAKE_CXX_FLAGS "${{CMAKE_CXX_FLAGS}} -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

# Linker script
set(LINKER_SCRIPT "${{CMAKE_CURRENT_SOURCE_DIR}}/linker/STM32F401RETx_FLASH.ld")
set(CMAKE_EXE_LINKER_FLAGS "${{CMAKE_EXE_LINKER_FLAGS}} -T${{LINKER_SCRIPT}}")

# Include directories
include_directories(
    include
    src/hal
    src/drivers
)

# Source files
file(GLOB_RECURSE SOURCES
    "src/*.c"
    "src/*.cpp"
    "startup/*.s"
)

# Create executable
add_executable({}.elf ${{SOURCES}})

# Generate additional files
add_custom_command(TARGET {}.elf POST_BUILD
    COMMAND ${{CMAKE_OBJCOPY}} -Oihex ${}.elf ${}.hex
    COMMAND ${{CMAKE_OBJCOPY}} -Obinary ${}.elf ${}.bin
    COMMAND ${{CMAKE_SIZE}} ${}.elf
    COMMENT "Generating HEX and BIN files"
)
)",
                       options_.projectName, options_.projectName, options_.projectName,
                       options_.projectName, options_.projectName, options_.projectName,
                       options_.projectName);
}

std::string EmbeddedTemplate::getMakefileContent() {
    return fmt::format(R"(# Makefile for {} embedded project

# Target configuration
TARGET = {}
MCU = cortex-m4
ARCH = armv7e-m

# Toolchain
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
AS = $(PREFIX)as
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump
SIZE = $(PREFIX)size

# Directories
SRCDIR = src
INCDIR = include
BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj

# Source files
CSOURCES = $(shell find $(SRCDIR) -name '*.c')
CXXSOURCES = $(shell find $(SRCDIR) -name '*.cpp')
ASMSOURCES = $(shell find startup -name '*.s')

# Object files
COBJECTS = $(CSOURCES:%.c=$(OBJDIR)/%.o)
CXXOBJECTS = $(CXXSOURCES:%.cpp=$(OBJDIR)/%.o)
ASMOBJECTS = $(ASMSOURCES:%.s=$(OBJDIR)/%.o)
OBJECTS = $(COBJECTS) $(CXXOBJECTS) $(ASMOBJECTS)

# Compiler flags
CFLAGS = -mcpu=$(MCU) -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS += -DUSE_HAL_DRIVER -DSTM32F401xE
CFLAGS += -I$(INCDIR) -Isrc/hal -Isrc/drivers
CFLAGS += -Wall -Wextra -O2 -g

CXXFLAGS = $(CFLAGS) -std=c++17 -fno-exceptions -fno-rtti

# Linker flags
LDFLAGS = -mcpu=$(MCU) -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard
LDFLAGS += -Tlinker/STM32F401RETx_FLASH.ld
LDFLAGS += -Wl,--gc-sections -Wl,-Map=$(BUILDDIR)/$(TARGET).map

# Default target
all: $(BUILDDIR)/$(TARGET).elf $(BUILDDIR)/$(TARGET).hex $(BUILDDIR)/$(TARGET).bin

# Create build directory
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/$(SRCDIR)
	mkdir -p $(OBJDIR)/startup

# Compile C sources
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C++ sources
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile assembly sources
$(OBJDIR)/%.o: %.s | $(OBJDIR)
	$(AS) -mcpu=$(MCU) -mthumb $< -o $@

# Link
$(BUILDDIR)/$(TARGET).elf: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@
	$(SIZE) $@

# Generate hex file
$(BUILDDIR)/$(TARGET).hex: $(BUILDDIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

# Generate binary file
$(BUILDDIR)/$(TARGET).bin: $(BUILDDIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Clean
clean:
	rm -rf $(BUILDDIR)

# Flash using OpenOCD
flash: $(BUILDDIR)/$(TARGET).elf
	openocd -f config/openocd.cfg -c "program $< verify reset exit"

# Debug
debug: $(BUILDDIR)/$(TARGET).elf
	openocd -f config/openocd.cfg &
	$(PREFIX)gdb $< -ex "target remote localhost:3333"

.PHONY: all clean flash debug
)",
                       options_.projectName, options_.projectName);
}

// Add remaining missing method implementations
std::string EmbeddedTemplate::getHalHeaderContent() {
    return R"(#ifndef HAL_H
#define HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// HAL Status definitions
typedef enum {
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

// GPIO definitions
typedef enum {
    GPIO_MODE_INPUT                 = 0x00000000U,
    GPIO_MODE_OUTPUT_PP             = 0x00000001U,
    GPIO_MODE_OUTPUT_OD             = 0x00000011U,
    GPIO_MODE_AF_PP                 = 0x00000002U,
    GPIO_MODE_AF_OD                 = 0x00000012U,
    GPIO_MODE_ANALOG                = 0x00000003U,
    GPIO_MODE_IT_RISING             = 0x10110000U,
    GPIO_MODE_IT_FALLING            = 0x10210000U,
    GPIO_MODE_IT_RISING_FALLING     = 0x10310000U,
    GPIO_MODE_EVT_RISING            = 0x10120000U,
    GPIO_MODE_EVT_FALLING           = 0x10220000U,
    GPIO_MODE_EVT_RISING_FALLING    = 0x10320000U
} GPIO_ModeTypeDef;

typedef enum {
    GPIO_NOPULL   = 0x00000000U,
    GPIO_PULLUP   = 0x00000001U,
    GPIO_PULLDOWN = 0x00000002U
} GPIO_PullTypeDef;

typedef enum {
    GPIO_SPEED_FREQ_LOW       = 0x00000000U,
    GPIO_SPEED_FREQ_MEDIUM    = 0x00000001U,
    GPIO_SPEED_FREQ_HIGH      = 0x00000002U,
    GPIO_SPEED_FREQ_VERY_HIGH = 0x00000003U
} GPIO_SpeedTypeDef;

typedef struct {
    uint32_t Pin;
    uint32_t Mode;
    uint32_t Pull;
    uint32_t Speed;
    uint32_t Alternate;
} GPIO_InitTypeDef;

// Function prototypes
HAL_StatusTypeDef HAL_Init(void);
void HAL_Delay(uint32_t Delay);
uint32_t HAL_GetTick(void);

#ifdef __cplusplus
}
#endif

#endif // HAL_H
)";
}

std::string EmbeddedTemplate::getHalCppContent() {
    return R"(#include "hal/hal.h"
#include "main.h"

HAL_StatusTypeDef HAL_Init(void) {
    // Initialize the HAL Library
    return HAL_OK;
}

void HAL_Delay(uint32_t Delay) {
    uint32_t tickstart = HAL_GetTick();
    uint32_t wait = Delay;

    if (wait < 0xFFFFFFFFU) {
        wait += 1U;
    }

    while ((HAL_GetTick() - tickstart) < wait) {
        // Wait
    }
}

uint32_t HAL_GetTick(void) {
    return g_systemTick;
}
)";
}

std::string EmbeddedTemplate::getGpioHeaderContent() {
    return R"(#ifndef GPIO_H
#define GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal/hal.h"

// GPIO pin definitions
#define GPIO_PIN_0                 ((uint16_t)0x0001)
#define GPIO_PIN_1                 ((uint16_t)0x0002)
#define GPIO_PIN_13                ((uint16_t)0x2000)

// GPIO port definitions
#define GPIOA                      ((void*)0x40020000)
#define GPIOB                      ((void*)0x40020400)
#define GPIOC                      ((void*)0x40020800)

// Function prototypes
void HAL_GPIO_Init(void* GPIOx, GPIO_InitTypeDef* GPIO_Init);
void HAL_GPIO_WritePin(void* GPIOx, uint16_t GPIO_Pin, uint32_t PinState);
void HAL_GPIO_TogglePin(void* GPIOx, uint16_t GPIO_Pin);
uint32_t HAL_GPIO_ReadPin(void* GPIOx, uint16_t GPIO_Pin);

// Convenience macros
#define GPIO_WritePin(port, pin, state) HAL_GPIO_WritePin(port, pin, state)
#define GPIO_TogglePin(port, pin) HAL_GPIO_TogglePin(port, pin)
#define GPIO_ReadPin(port, pin) HAL_GPIO_ReadPin(port, pin)

#ifdef __cplusplus
}
#endif

#endif // GPIO_H
)";
}

std::string EmbeddedTemplate::getGpioCppContent() {
    return R"(#include "hal/gpio.h"

void HAL_GPIO_Init(void* GPIOx, GPIO_InitTypeDef* GPIO_Init) {
    // GPIO initialization implementation
    // This would typically configure the GPIO registers
    (void)GPIOx;
    (void)GPIO_Init;
}

void HAL_GPIO_WritePin(void* GPIOx, uint16_t GPIO_Pin, uint32_t PinState) {
    // GPIO write implementation
    (void)GPIOx;
    (void)GPIO_Pin;
    (void)PinState;
}

void HAL_GPIO_TogglePin(void* GPIOx, uint16_t GPIO_Pin) {
    // GPIO toggle implementation
    (void)GPIOx;
    (void)GPIO_Pin;
}

uint32_t HAL_GPIO_ReadPin(void* GPIOx, uint16_t GPIO_Pin) {
    // GPIO read implementation
    (void)GPIOx;
    (void)GPIO_Pin;
    return 0;
}
)";
}

std::string EmbeddedTemplate::getUnityTestContent() {
    return R"(#include "unity.h"
#include "main.h"

void setUp(void) {
    // Set up test environment
}

void tearDown(void) {
    // Clean up after test
}

void test_system_initialization(void) {
    // Test system initialization
    TEST_ASSERT_TRUE(true);
}

void test_gpio_functionality(void) {
    // Test GPIO functionality
    TEST_ASSERT_TRUE(true);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_system_initialization);
    RUN_TEST(test_gpio_functionality);

    return UNITY_END();
}
)";
}

std::string EmbeddedTemplate::getHardwareTestContent() {
    return R"(#include "unity.h"
#include "hal/gpio.h"
#include "main.h"

void setUp(void) {
    // Initialize hardware for testing
    SystemInit();
    GPIO_Init();
}

void tearDown(void) {
    // Clean up hardware after test
}

void test_led_toggle(void) {
    // Test LED toggle functionality
    GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    HAL_Delay(100);
    TEST_ASSERT_TRUE(true);
}

void test_gpio_read_write(void) {
    // Test GPIO read/write operations
    GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
    HAL_Delay(10);
    uint32_t state = GPIO_ReadPin(LED_GPIO_Port, LED_Pin);
    TEST_ASSERT_EQUAL(1, state);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_led_toggle);
    RUN_TEST(test_gpio_read_write);

    return UNITY_END();
}
)";
}

std::string EmbeddedTemplate::getOpenOCDConfigContent() {
    return R"(# OpenOCD configuration for STM32F4 Discovery
source [find interface/stlink.cfg]
source [find target/stm32f4x.cfg]

# Reset configuration
reset_config srst_only

# Flash configuration
flash bank $_FLASHNAME stm32f2x 0x08000000 0 0 0 $_TARGETNAME

# Adapter speed
adapter speed 2000

# Enable semihosting
arm semihosting enable

# GDB configuration
gdb_port 3333
telnet_port 4444
tcl_port 6666
)";
}

std::string EmbeddedTemplate::getGdbInitContent() {
    return R"(# GDB initialization script
target remote localhost:3333

# Load symbols
file build/firmware.elf

# Set up breakpoints
# break main
# break Error_Handler

# Monitor commands
monitor reset halt
monitor flash write_image erase build/firmware.elf

# Start execution
# continue
)";
}
