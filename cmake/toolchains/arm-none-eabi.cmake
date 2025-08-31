# ARM Cortex-M embedded toolchain file for CMake
# This toolchain file configures CMake for ARM Cortex-M cross-compilation

cmake_minimum_required(VERSION 3.20)

# Set the system name and processor
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# ARM toolchain configuration
if(NOT ARM_TOOLCHAIN_PATH)
    # Try to find arm-none-eabi-gcc in PATH
    find_program(ARM_GCC_EXECUTABLE arm-none-eabi-gcc)
    if(ARM_GCC_EXECUTABLE)
        get_filename_component(ARM_TOOLCHAIN_PATH "${ARM_GCC_EXECUTABLE}" DIRECTORY)
    else()
        message(FATAL_ERROR "ARM toolchain not found. Please install arm-none-eabi-gcc and set ARM_TOOLCHAIN_PATH")
    endif()
endif()

# Set default target if not specified
if(NOT ARM_TARGET)
    set(ARM_TARGET "cortex-m4") # cortex-m0, cortex-m0plus, cortex-m3, cortex-m4, cortex-m7, cortex-m33
endif()

# Set default FPU if not specified
if(NOT ARM_FPU)
    if(ARM_TARGET MATCHES "cortex-m4|cortex-m7")
        set(ARM_FPU "fpv4-sp-d16")
    elseif(ARM_TARGET MATCHES "cortex-m33")
        set(ARM_FPU "fpv5-sp-d16")
    else()
        set(ARM_FPU "")
    endif()
endif()

# Set default float ABI
if(NOT ARM_FLOAT_ABI)
    if(ARM_FPU)
        set(ARM_FLOAT_ABI "hard")
    else()
        set(ARM_FLOAT_ABI "soft")
    endif()
endif()

# Set compilers and tools
set(CMAKE_C_COMPILER "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-gcc")
set(CMAKE_AR "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-ar")
set(CMAKE_RANLIB "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-ranlib")
set(CMAKE_STRIP "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-strip")
set(CMAKE_OBJCOPY "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-objcopy")
set(CMAKE_OBJDUMP "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-objdump")
set(CMAKE_SIZE "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-size")

# Verify compilers exist
if(NOT EXISTS "${CMAKE_C_COMPILER}")
    message(FATAL_ERROR "ARM C compiler not found: ${CMAKE_C_COMPILER}")
endif()

if(NOT EXISTS "${CMAKE_CXX_COMPILER}")
    message(FATAL_ERROR "ARM C++ compiler not found: ${CMAKE_CXX_COMPILER}")
endif()

# Set find root path
set(CMAKE_FIND_ROOT_PATH "${ARM_TOOLCHAIN_PATH}/../arm-none-eabi")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# ARM Cortex-M specific compiler flags
set(ARM_COMPILER_FLAGS "")
set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -mcpu=${ARM_TARGET}")
set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -mthumb")

# Add FPU flags if specified
if(ARM_FPU)
    set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -mfpu=${ARM_FPU}")
    set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -mfloat-abi=${ARM_FLOAT_ABI}")
endif()

# Common embedded flags
set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -ffunction-sections")
set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -fdata-sections")
set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -fno-common")
set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -fmessage-length=0")
set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -fno-exceptions")
set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -fno-rtti")
set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -fno-threadsafe-statics")
set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -fno-use-cxa-atexit")

# Set compiler flags
set(CMAKE_C_FLAGS_INIT "${ARM_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${ARM_COMPILER_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${ARM_COMPILER_FLAGS} -x assembler-with-cpp")

# ARM Cortex-M specific linker flags
set(ARM_LINKER_FLAGS "")
set(ARM_LINKER_FLAGS "${ARM_LINKER_FLAGS} -mcpu=${ARM_TARGET}")
set(ARM_LINKER_FLAGS "${ARM_LINKER_FLAGS} -mthumb")

if(ARM_FPU)
    set(ARM_LINKER_FLAGS "${ARM_LINKER_FLAGS} -mfpu=${ARM_FPU}")
    set(ARM_LINKER_FLAGS "${ARM_LINKER_FLAGS} -mfloat-abi=${ARM_FLOAT_ABI}")
endif()

set(ARM_LINKER_FLAGS "${ARM_LINKER_FLAGS} -Wl,--gc-sections")
set(ARM_LINKER_FLAGS "${ARM_LINKER_FLAGS} -Wl,--print-memory-usage")
set(ARM_LINKER_FLAGS "${ARM_LINKER_FLAGS} -Wl,--no-wchar-size-warning")
set(ARM_LINKER_FLAGS "${ARM_LINKER_FLAGS} --specs=nano.specs")
set(ARM_LINKER_FLAGS "${ARM_LINKER_FLAGS} --specs=nosys.specs")

# Set linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "${ARM_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${ARM_LINKER_FLAGS}")

# Set build type specific flags
set(CMAKE_C_FLAGS_DEBUG_INIT "-g -O0")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -O0")
set(CMAKE_C_FLAGS_RELEASE_INIT "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")

# Embedded-specific definitions
add_definitions(-DARM_MATH_CM4)
add_definitions(-D__FPU_PRESENT=1)
add_definitions(-D__FPU_USED=1)

# Set properties for embedded build
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# Disable shared libraries (not supported in embedded)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_SHARED_LIBRARY_SUFFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")
set(CMAKE_STATIC_LIBRARY_PREFIX "lib")

# Function to add linker script
function(target_add_linker_script target_name linker_script)
    if(NOT EXISTS "${linker_script}")
        message(FATAL_ERROR "Linker script not found: ${linker_script}")
    endif()
    
    target_link_options(${target_name} PRIVATE "-T${linker_script}")
endfunction()

# Function to generate binary and hex files
function(target_generate_binary target_name)
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${target_name}> ${target_name}.bin
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${target_name}> ${target_name}.hex
        COMMENT "Generating binary and hex files for ${target_name}"
    )
    
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${target_name}>
        COMMENT "Memory usage for ${target_name}:"
    )
endfunction()

# Function to configure ARM Cortex-M target
function(configure_arm_target target_name)
    set(options GENERATE_MAP PRINT_SIZE)
    set(oneValueArgs LINKER_SCRIPT HEAP_SIZE STACK_SIZE)
    set(multiValueArgs)
    cmake_parse_arguments(ARM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Add linker script if provided
    if(ARM_LINKER_SCRIPT)
        target_add_linker_script(${target_name} "${ARM_LINKER_SCRIPT}")
    endif()
    
    # Set heap and stack sizes
    if(ARM_HEAP_SIZE)
        target_compile_definitions(${target_name} PRIVATE HEAP_SIZE=${ARM_HEAP_SIZE})
    endif()
    
    if(ARM_STACK_SIZE)
        target_compile_definitions(${target_name} PRIVATE STACK_SIZE=${ARM_STACK_SIZE})
    endif()
    
    # Generate map file
    if(ARM_GENERATE_MAP)
        target_link_options(${target_name} PRIVATE "-Wl,-Map=${target_name}.map")
    endif()
    
    # Generate binary files
    target_generate_binary(${target_name})
    
    # Print size information
    if(ARM_PRINT_SIZE)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${target_name}>
            COMMENT "Size information for ${target_name}:"
        )
    endif()
endfunction()

# Function to add startup file
function(target_add_startup_file target_name startup_file)
    if(NOT EXISTS "${startup_file}")
        message(FATAL_ERROR "Startup file not found: ${startup_file}")
    endif()
    
    target_sources(${target_name} PRIVATE "${startup_file}")
    set_source_files_properties("${startup_file}" PROPERTIES LANGUAGE ASM)
endfunction()

# Get toolchain version
execute_process(
    COMMAND "${CMAKE_C_COMPILER}" --version
    OUTPUT_VARIABLE ARM_TOOLCHAIN_VERSION_OUTPUT
    ERROR_QUIET
)

if(ARM_TOOLCHAIN_VERSION_OUTPUT MATCHES "gcc version ([0-9]+\\.[0-9]+\\.[0-9]+)")
    set(ARM_TOOLCHAIN_VERSION "${CMAKE_MATCH_1}")
endif()

message(STATUS "ARM Cortex-M toolchain configured:")
message(STATUS "  Toolchain Path: ${ARM_TOOLCHAIN_PATH}")
message(STATUS "  Version: ${ARM_TOOLCHAIN_VERSION}")
message(STATUS "  Target: ${ARM_TARGET}")
message(STATUS "  FPU: ${ARM_FPU}")
message(STATUS "  Float ABI: ${ARM_FLOAT_ABI}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  C++ Compiler: ${CMAKE_CXX_COMPILER}")
