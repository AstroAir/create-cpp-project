# test-embedded

An embedded C++ project for microcontrollers.

## Features

- Hardware Abstraction Layer (HAL)
- GPIO control
- UART communication
- SPI/I2C support
- FreeRTOS integration (optional)
- Debugging support with OpenOCD/GDB

## Hardware Requirements

- Target MCU: STM32F4
- Development Board: STM32F4-Discovery
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
openocd -f config/openocd.cfg -c "program build/test-embedded.elf verify reset exit"

# Using ST-Link
st-flash write build/test-embedded.bin 0x8000000
```

## Debugging

```bash
# Start OpenOCD server
openocd -f config/openocd.cfg

# In another terminal, start GDB
arm-none-eabi-gdb build/test-embedded.elf
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
