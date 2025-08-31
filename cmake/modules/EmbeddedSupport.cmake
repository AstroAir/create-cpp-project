# EmbeddedSupport.cmake
# Comprehensive embedded systems development support

# Function to detect ARM toolchain
function(detect_arm_toolchain)
    # Try to find ARM toolchain in various locations
    set(ARM_TOOLCHAIN_SEARCH_PATHS
        "$ENV{ARM_TOOLCHAIN_PATH}"
        "/usr/bin"
        "/usr/local/bin"
        "/opt/gcc-arm-none-eabi/bin"
        "C:/Program Files (x86)/GNU Arm Embedded Toolchain/*/bin"
        "C:/Program Files/GNU Arm Embedded Toolchain/*/bin"
    )

    foreach(SEARCH_PATH ${ARM_TOOLCHAIN_SEARCH_PATHS})
        if(EXISTS "${SEARCH_PATH}/arm-none-eabi-gcc")
            set(ARM_TOOLCHAIN_PATH "${SEARCH_PATH}" PARENT_SCOPE)
            message(STATUS "Found ARM toolchain at: ${SEARCH_PATH}")
            return()
        endif()
    endforeach()

    # Try to find arm-none-eabi-gcc in PATH
    find_program(ARM_GCC_EXECUTABLE arm-none-eabi-gcc)
    if(ARM_GCC_EXECUTABLE)
        get_filename_component(ARM_TOOLCHAIN_PATH "${ARM_GCC_EXECUTABLE}" DIRECTORY)
        set(ARM_TOOLCHAIN_PATH "${ARM_TOOLCHAIN_PATH}" PARENT_SCOPE)
        message(STATUS "Found ARM toolchain in PATH: ${ARM_TOOLCHAIN_PATH}")
        return()
    endif()

    message(WARNING "ARM toolchain not found. Embedded builds will not be available.")
    set(ARM_TOOLCHAIN_AVAILABLE FALSE PARENT_SCOPE)
endfunction()

# Function to configure embedded target
function(configure_embedded_target target_name)
    set(options GENERATE_HEX GENERATE_BIN GENERATE_MAP PRINT_SIZE)
    set(oneValueArgs 
        MCU_TYPE 
        LINKER_SCRIPT 
        HEAP_SIZE 
        STACK_SIZE 
        FLASH_SIZE 
        RAM_SIZE
        OPTIMIZATION_LEVEL
    )
    set(multiValueArgs 
        STARTUP_FILES 
        DEFINITIONS 
        INCLUDE_DIRS
        LINK_FLAGS
        COMPILE_FLAGS
    )
    cmake_parse_arguments(EMB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Generic")
        message(WARNING "configure_embedded_target called but not building for embedded system")
        return()
    endif()

    # Set default values
    if(NOT EMB_MCU_TYPE)
        set(EMB_MCU_TYPE "cortex-m4")
    endif()

    if(NOT EMB_HEAP_SIZE)
        set(EMB_HEAP_SIZE "0x1000")  # 4KB
    endif()

    if(NOT EMB_STACK_SIZE)
        set(EMB_STACK_SIZE "0x2000")  # 8KB
    endif()

    if(NOT EMB_OPTIMIZATION_LEVEL)
        set(EMB_OPTIMIZATION_LEVEL "s")  # Optimize for size
    endif()

    # Add MCU-specific definitions
    target_compile_definitions(${target_name} PRIVATE
        ${EMB_DEFINITIONS}
        HEAP_SIZE=${EMB_HEAP_SIZE}
        STACK_SIZE=${EMB_STACK_SIZE}
    )

    if(EMB_FLASH_SIZE)
        target_compile_definitions(${target_name} PRIVATE FLASH_SIZE=${EMB_FLASH_SIZE})
    endif()

    if(EMB_RAM_SIZE)
        target_compile_definitions(${target_name} PRIVATE RAM_SIZE=${EMB_RAM_SIZE})
    endif()

    # Add include directories
    if(EMB_INCLUDE_DIRS)
        target_include_directories(${target_name} PRIVATE ${EMB_INCLUDE_DIRS})
    endif()

    # Add startup files
    if(EMB_STARTUP_FILES)
        foreach(STARTUP_FILE ${EMB_STARTUP_FILES})
            if(EXISTS "${STARTUP_FILE}")
                target_sources(${target_name} PRIVATE "${STARTUP_FILE}")
                set_source_files_properties("${STARTUP_FILE}" PROPERTIES LANGUAGE ASM)
            else()
                message(WARNING "Startup file not found: ${STARTUP_FILE}")
            endif()
        endforeach()
    endif()

    # Add linker script
    if(EMB_LINKER_SCRIPT)
        if(EXISTS "${EMB_LINKER_SCRIPT}")
            target_link_options(${target_name} PRIVATE "-T${EMB_LINKER_SCRIPT}")
        else()
            message(FATAL_ERROR "Linker script not found: ${EMB_LINKER_SCRIPT}")
        endif()
    endif()

    # Add compile flags
    if(EMB_COMPILE_FLAGS)
        target_compile_options(${target_name} PRIVATE ${EMB_COMPILE_FLAGS})
    endif()

    # Add link flags
    set(DEFAULT_LINK_FLAGS
        "-Wl,--gc-sections"
        "-Wl,--print-memory-usage"
        "--specs=nano.specs"
        "--specs=nosys.specs"
    )
    
    if(EMB_GENERATE_MAP)
        list(APPEND DEFAULT_LINK_FLAGS "-Wl,-Map=${target_name}.map")
    endif()

    target_link_options(${target_name} PRIVATE 
        ${DEFAULT_LINK_FLAGS}
        ${EMB_LINK_FLAGS}
    )

    # Set optimization level
    target_compile_options(${target_name} PRIVATE "-O${EMB_OPTIMIZATION_LEVEL}")

    # Generate binary files
    if(EMB_GENERATE_BIN OR EMB_GENERATE_HEX)
        find_program(ARM_OBJCOPY arm-none-eabi-objcopy)
        if(ARM_OBJCOPY)
            if(EMB_GENERATE_BIN)
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${ARM_OBJCOPY} -O binary $<TARGET_FILE:${target_name}> ${target_name}.bin
                    COMMENT "Generating binary file: ${target_name}.bin"
                )
            endif()

            if(EMB_GENERATE_HEX)
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${ARM_OBJCOPY} -O ihex $<TARGET_FILE:${target_name}> ${target_name}.hex
                    COMMENT "Generating hex file: ${target_name}.hex"
                )
            endif()
        endif()
    endif()

    # Print size information
    if(EMB_PRINT_SIZE)
        find_program(ARM_SIZE arm-none-eabi-size)
        if(ARM_SIZE)
            add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND ${ARM_SIZE} --format=berkeley $<TARGET_FILE:${target_name}>
                COMMENT "Memory usage for ${target_name}:"
            )
        endif()
    endif()

    message(STATUS "Configured embedded target: ${target_name} (${EMB_MCU_TYPE})")
endfunction()

# Function to create embedded library
function(create_embedded_library target_name)
    set(options STATIC_ONLY)
    set(oneValueArgs MCU_TYPE)
    set(multiValueArgs SOURCES HEADERS DEFINITIONS)
    cmake_parse_arguments(LIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Generic")
        message(WARNING "create_embedded_library called but not building for embedded system")
        return()
    endif()

    # Create static library (shared libraries not supported in embedded)
    add_library(${target_name} STATIC ${LIB_SOURCES} ${LIB_HEADERS})

    # Add definitions
    if(LIB_DEFINITIONS)
        target_compile_definitions(${target_name} PRIVATE ${LIB_DEFINITIONS})
    endif()

    # Configure for embedded
    if(LIB_MCU_TYPE)
        configure_embedded_target(${target_name} MCU_TYPE ${LIB_MCU_TYPE})
    endif()

    message(STATUS "Created embedded library: ${target_name}")
endfunction()

# Function to add embedded optimization flags
function(add_embedded_optimizations target_name)
    set(options SIZE_OPTIMIZED SPEED_OPTIMIZED DEBUG_OPTIMIZED)
    set(oneValueArgs)
    set(multiValueArgs)
    cmake_parse_arguments(OPT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Generic")
        return()
    endif()

    if(OPT_SIZE_OPTIMIZED)
        target_compile_options(${target_name} PRIVATE 
            "-Os"
            "-ffunction-sections"
            "-fdata-sections"
            "-fno-common"
            "-fno-exceptions"
            "-fno-rtti"
            "-fno-unwind-tables"
        )
    elseif(OPT_SPEED_OPTIMIZED)
        target_compile_options(${target_name} PRIVATE 
            "-O2"
            "-ffunction-sections"
            "-fdata-sections"
            "-fno-common"
        )
    elseif(OPT_DEBUG_OPTIMIZED)
        target_compile_options(${target_name} PRIVATE 
            "-O0"
            "-g3"
            "-gdwarf-2"
        )
    endif()
endfunction()

# Function to configure MCU-specific settings
function(configure_mcu_settings target_name mcu_type)
    # MCU-specific configurations
    if(mcu_type MATCHES "cortex-m0")
        target_compile_options(${target_name} PRIVATE 
            "-mcpu=cortex-m0"
            "-mthumb"
            "-mfloat-abi=soft"
        )
        target_compile_definitions(${target_name} PRIVATE 
            ARM_MATH_CM0
            __CORTEX_M=0
        )
    elseif(mcu_type MATCHES "cortex-m0plus")
        target_compile_options(${target_name} PRIVATE 
            "-mcpu=cortex-m0plus"
            "-mthumb"
            "-mfloat-abi=soft"
        )
        target_compile_definitions(${target_name} PRIVATE 
            ARM_MATH_CM0PLUS
            __CORTEX_M=0
        )
    elseif(mcu_type MATCHES "cortex-m3")
        target_compile_options(${target_name} PRIVATE 
            "-mcpu=cortex-m3"
            "-mthumb"
            "-mfloat-abi=soft"
        )
        target_compile_definitions(${target_name} PRIVATE 
            ARM_MATH_CM3
            __CORTEX_M=3
        )
    elseif(mcu_type MATCHES "cortex-m4")
        target_compile_options(${target_name} PRIVATE 
            "-mcpu=cortex-m4"
            "-mthumb"
            "-mfpu=fpv4-sp-d16"
            "-mfloat-abi=hard"
        )
        target_compile_definitions(${target_name} PRIVATE 
            ARM_MATH_CM4
            __CORTEX_M=4
            __FPU_PRESENT=1
            __FPU_USED=1
        )
    elseif(mcu_type MATCHES "cortex-m7")
        target_compile_options(${target_name} PRIVATE 
            "-mcpu=cortex-m7"
            "-mthumb"
            "-mfpu=fpv5-d16"
            "-mfloat-abi=hard"
        )
        target_compile_definitions(${target_name} PRIVATE 
            ARM_MATH_CM7
            __CORTEX_M=7
            __FPU_PRESENT=1
            __FPU_USED=1
        )
    elseif(mcu_type MATCHES "cortex-m33")
        target_compile_options(${target_name} PRIVATE 
            "-mcpu=cortex-m33"
            "-mthumb"
            "-mfpu=fpv5-sp-d16"
            "-mfloat-abi=hard"
        )
        target_compile_definitions(${target_name} PRIVATE 
            ARM_MATH_CM33
            __CORTEX_M=33
            __FPU_PRESENT=1
            __FPU_USED=1
        )
    else()
        message(WARNING "Unknown MCU type: ${mcu_type}")
    endif()

    message(STATUS "Configured MCU settings for: ${mcu_type}")
endfunction()

# Function to setup embedded development environment
function(setup_embedded_environment)
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Generic")
        return()
    endif()

    detect_arm_toolchain()

    if(NOT ARM_TOOLCHAIN_AVAILABLE)
        return()
    endif()

    # Set common embedded definitions
    add_definitions(-DEMBEDDED)
    add_definitions(-DNO_STDLIB)

    # Disable shared libraries
    set(BUILD_SHARED_LIBS OFF PARENT_SCOPE)
    set(CMAKE_EXECUTABLE_SUFFIX ".elf" PARENT_SCOPE)

    message(STATUS "Embedded development environment configured")
endfunction()

# Function to create flash programming target
function(create_flash_target target_name)
    set(options)
    set(oneValueArgs PROGRAMMER INTERFACE DEVICE)
    set(multiValueArgs PROGRAMMER_ARGS)
    cmake_parse_arguments(FLASH "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Generic")
        return()
    endif()

    # Default to OpenOCD if no programmer specified
    if(NOT FLASH_PROGRAMMER)
        set(FLASH_PROGRAMMER "openocd")
    endif()

    if(FLASH_PROGRAMMER STREQUAL "openocd")
        find_program(OPENOCD_EXECUTABLE openocd)
        if(OPENOCD_EXECUTABLE)
            set(FLASH_CMD ${OPENOCD_EXECUTABLE})
            if(FLASH_INTERFACE)
                list(APPEND FLASH_CMD "-f" "interface/${FLASH_INTERFACE}.cfg")
            endif()
            if(FLASH_DEVICE)
                list(APPEND FLASH_CMD "-f" "target/${FLASH_DEVICE}.cfg")
            endif()
            list(APPEND FLASH_CMD "-c" "program $<TARGET_FILE:${target_name}> verify reset exit")

            add_custom_target(flash-${target_name}
                COMMAND ${FLASH_CMD}
                DEPENDS ${target_name}
                COMMENT "Flashing ${target_name} to device"
            )
            
            message(STATUS "Created flash target: flash-${target_name}")
        else()
            message(WARNING "OpenOCD not found. Flash target will not be available.")
        endif()
    endif()
endfunction()
