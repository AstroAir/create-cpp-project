# WebAssembly/Emscripten toolchain file for CMake
# This toolchain file configures CMake for WebAssembly compilation using Emscripten

cmake_minimum_required(VERSION 3.20)

# Set the system name
set(CMAKE_SYSTEM_NAME Emscripten)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR x86)

# Emscripten SDK configuration
if(NOT EMSCRIPTEN_ROOT_PATH)
    if(DEFINED ENV{EMSDK})
        set(EMSCRIPTEN_ROOT_PATH "$ENV{EMSDK}/upstream/emscripten")
    elseif(DEFINED ENV{EMSCRIPTEN})
        set(EMSCRIPTEN_ROOT_PATH "$ENV{EMSCRIPTEN}")
    else()
        # Try to find emcc in PATH
        find_program(EMCC_EXECUTABLE emcc)
        if(EMCC_EXECUTABLE)
            get_filename_component(EMSCRIPTEN_ROOT_PATH "${EMCC_EXECUTABLE}" DIRECTORY)
        else()
            message(FATAL_ERROR "Emscripten not found. Please install Emscripten SDK and set EMSDK environment variable")
        endif()
    endif()
endif()

# Validate Emscripten path
if(NOT EXISTS "${EMSCRIPTEN_ROOT_PATH}")
    message(FATAL_ERROR "Emscripten not found at: ${EMSCRIPTEN_ROOT_PATH}")
endif()

# Set compilers
if(WIN32)
    set(EMCC_SUFFIX ".bat")
else()
    set(EMCC_SUFFIX "")
endif()

set(CMAKE_C_COMPILER "${EMSCRIPTEN_ROOT_PATH}/emcc${EMCC_SUFFIX}")
set(CMAKE_CXX_COMPILER "${EMSCRIPTEN_ROOT_PATH}/em++${EMCC_SUFFIX}")
set(CMAKE_AR "${EMSCRIPTEN_ROOT_PATH}/emar${EMCC_SUFFIX}")
set(CMAKE_RANLIB "${EMSCRIPTEN_ROOT_PATH}/emranlib${EMCC_SUFFIX}")

# Verify compilers exist
if(NOT EXISTS "${CMAKE_C_COMPILER}")
    message(FATAL_ERROR "Emscripten C compiler not found: ${CMAKE_C_COMPILER}")
endif()

if(NOT EXISTS "${CMAKE_CXX_COMPILER}")
    message(FATAL_ERROR "Emscripten C++ compiler not found: ${CMAKE_CXX_COMPILER}")
endif()

# Set find root path
set(CMAKE_FIND_ROOT_PATH "${EMSCRIPTEN_ROOT_PATH}/system")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# WebAssembly target configuration
if(NOT WASM_TARGET)
    set(WASM_TARGET "web") # web, node, worker
endif()

if(NOT WASM_MEMORY_SIZE)
    set(WASM_MEMORY_SIZE "128MB")
endif()

if(NOT WASM_STACK_SIZE)
    set(WASM_STACK_SIZE "64KB")
endif()

# WebAssembly-specific compiler flags
set(WASM_COMPILER_FLAGS "")
set(WASM_COMPILER_FLAGS "${WASM_COMPILER_FLAGS} -fno-exceptions")
set(WASM_COMPILER_FLAGS "${WASM_COMPILER_FLAGS} -fno-rtti")
set(WASM_COMPILER_FLAGS "${WASM_COMPILER_FLAGS} -fno-threadsafe-statics")

# Optimization flags
if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
    set(WASM_COMPILER_FLAGS "${WASM_COMPILER_FLAGS} -O3")
    set(WASM_COMPILER_FLAGS "${WASM_COMPILER_FLAGS} -flto")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(WASM_COMPILER_FLAGS "${WASM_COMPILER_FLAGS} -O0")
    set(WASM_COMPILER_FLAGS "${WASM_COMPILER_FLAGS} -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(WASM_COMPILER_FLAGS "${WASM_COMPILER_FLAGS} -Oz")
    set(WASM_COMPILER_FLAGS "${WASM_COMPILER_FLAGS} -flto")
endif()

# Set compiler flags
set(CMAKE_C_FLAGS_INIT "${WASM_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${WASM_COMPILER_FLAGS}")

# WebAssembly-specific linker flags
set(WASM_LINKER_FLAGS "")
set(WASM_LINKER_FLAGS "${WASM_LINKER_FLAGS} -s WASM=1")
set(WASM_LINKER_FLAGS "${WASM_LINKER_FLAGS} -s ALLOW_MEMORY_GROWTH=1")
set(WASM_LINKER_FLAGS "${WASM_LINKER_FLAGS} -s INITIAL_MEMORY=${WASM_MEMORY_SIZE}")
set(WASM_LINKER_FLAGS "${WASM_LINKER_FLAGS} -s STACK_SIZE=${WASM_STACK_SIZE}")

# Target-specific flags
if(WASM_TARGET STREQUAL "web")
    set(WASM_LINKER_FLAGS "${WASM_LINKER_FLAGS} -s ENVIRONMENT=web")
    set(WASM_LINKER_FLAGS "${WASM_LINKER_FLAGS} -s EXPORT_NAME='Module'")
    set(WASM_LINKER_FLAGS "${WASM_LINKER_FLAGS} -s MODULARIZE=1")
elseif(WASM_TARGET STREQUAL "node")
    set(WASM_LINKER_FLAGS "${WASM_LINKER_FLAGS} -s ENVIRONMENT=node")
    set(WASM_LINKER_FLAGS "${WASM_LINKER_FLAGS} -s NODEJS_CATCH_EXIT=0")
elseif(WASM_TARGET STREQUAL "worker")
    set(WASM_LINKER_FLAGS "${WASM_LINKER_FLAGS} -s ENVIRONMENT=worker")
endif()

# Additional useful flags
set(WASM_LINKER_FLAGS "${WASM_LINKER_FLAGS} -s NO_EXIT_RUNTIME=1")
set(WASM_LINKER_FLAGS "${WASM_LINKER_FLAGS} -s ASSERTIONS=0")
set(WASM_LINKER_FLAGS "${WASM_LINKER_FLAGS} -s MALLOC=emmalloc")

# Set linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "${WASM_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${WASM_LINKER_FLAGS}")

# Set build type specific flags
set(CMAKE_C_FLAGS_DEBUG_INIT "-g -O0 -s ASSERTIONS=1 -s SAFE_HEAP=1")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -O0 -s ASSERTIONS=1 -s SAFE_HEAP=1")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG -s ASSERTIONS=0")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG -s ASSERTIONS=0")
set(CMAKE_C_FLAGS_MINSIZEREL_INIT "-Oz -DNDEBUG -s ASSERTIONS=0")
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-Oz -DNDEBUG -s ASSERTIONS=0")

# WebAssembly-specific definitions
add_definitions(-DEMSCRIPTEN)
add_definitions(-D__EMSCRIPTEN__)

# Set properties for WebAssembly build
set(EMSCRIPTEN TRUE)
set(CMAKE_CROSSCOMPILING TRUE)

# Set executable suffix
set(CMAKE_EXECUTABLE_SUFFIX ".js")

# Disable shared libraries (not supported in WebAssembly)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_SHARED_LIBRARY_SUFFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")
set(CMAKE_STATIC_LIBRARY_PREFIX "lib")

# Function to configure WebAssembly target
function(configure_wasm_target target_name)
    set(options ASYNCIFY PTHREAD SIMD)
    set(oneValueArgs SHELL_FILE PRELOAD_FILE EMBED_FILE)
    set(multiValueArgs EXPORTED_FUNCTIONS EXPORTED_RUNTIME_METHODS)
    cmake_parse_arguments(WASM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Additional flags for specific features
    set(TARGET_FLAGS "")
    
    if(WASM_ASYNCIFY)
        set(TARGET_FLAGS "${TARGET_FLAGS} -s ASYNCIFY=1")
    endif()
    
    if(WASM_PTHREAD)
        set(TARGET_FLAGS "${TARGET_FLAGS} -s USE_PTHREADS=1")
        set(TARGET_FLAGS "${TARGET_FLAGS} -s PTHREAD_POOL_SIZE=4")
    endif()
    
    if(WASM_SIMD)
        set(TARGET_FLAGS "${TARGET_FLAGS} -msimd128")
        set(TARGET_FLAGS "${TARGET_FLAGS} -s SIMD=1")
    endif()
    
    if(WASM_SHELL_FILE)
        set(TARGET_FLAGS "${TARGET_FLAGS} --shell-file ${WASM_SHELL_FILE}")
    endif()
    
    if(WASM_PRELOAD_FILE)
        set(TARGET_FLAGS "${TARGET_FLAGS} --preload-file ${WASM_PRELOAD_FILE}")
    endif()
    
    if(WASM_EMBED_FILE)
        set(TARGET_FLAGS "${TARGET_FLAGS} --embed-file ${WASM_EMBED_FILE}")
    endif()
    
    if(WASM_EXPORTED_FUNCTIONS)
        string(REPLACE ";" "," EXPORTED_FUNCS "${WASM_EXPORTED_FUNCTIONS}")
        set(TARGET_FLAGS "${TARGET_FLAGS} -s EXPORTED_FUNCTIONS=[${EXPORTED_FUNCS}]")
    endif()
    
    if(WASM_EXPORTED_RUNTIME_METHODS)
        string(REPLACE ";" "," EXPORTED_METHODS "${WASM_EXPORTED_RUNTIME_METHODS}")
        set(TARGET_FLAGS "${TARGET_FLAGS} -s EXPORTED_RUNTIME_METHODS=[${EXPORTED_METHODS}]")
    endif()
    
    # Apply flags to target
    if(TARGET_FLAGS)
        target_link_options(${target_name} PRIVATE ${TARGET_FLAGS})
    endif()
endfunction()

# Get Emscripten version
execute_process(
    COMMAND "${CMAKE_C_COMPILER}" --version
    OUTPUT_VARIABLE EMSCRIPTEN_VERSION_OUTPUT
    ERROR_QUIET
)

if(EMSCRIPTEN_VERSION_OUTPUT MATCHES "emcc \\(Emscripten gcc/clang-like replacement\\) ([0-9]+\\.[0-9]+\\.[0-9]+)")
    set(EMSCRIPTEN_VERSION "${CMAKE_MATCH_1}")
endif()

message(STATUS "WebAssembly/Emscripten toolchain configured:")
message(STATUS "  Emscripten Root: ${EMSCRIPTEN_ROOT_PATH}")
message(STATUS "  Version: ${EMSCRIPTEN_VERSION}")
message(STATUS "  Target: ${WASM_TARGET}")
message(STATUS "  Memory Size: ${WASM_MEMORY_SIZE}")
message(STATUS "  Stack Size: ${WASM_STACK_SIZE}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  C++ Compiler: ${CMAKE_CXX_COMPILER}")
