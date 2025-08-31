# WebAssemblySupport.cmake
# Comprehensive WebAssembly compilation support using Emscripten

# Function to detect Emscripten SDK
function(detect_emscripten_sdk)
    # Try to find Emscripten in various locations
    set(EMSCRIPTEN_SEARCH_PATHS
        "$ENV{EMSDK}/upstream/emscripten"
        "$ENV{EMSCRIPTEN}"
        "/opt/emsdk/upstream/emscripten"
        "/usr/local/emsdk/upstream/emscripten"
        "C:/emsdk/upstream/emscripten"
    )

    foreach(SEARCH_PATH ${EMSCRIPTEN_SEARCH_PATHS})
        if(EXISTS "${SEARCH_PATH}/emcc")
            set(EMSCRIPTEN_ROOT_PATH "${SEARCH_PATH}" PARENT_SCOPE)
            message(STATUS "Found Emscripten SDK at: ${SEARCH_PATH}")
            return()
        endif()
    endforeach()

    # Try to find emcc in PATH
    find_program(EMCC_EXECUTABLE emcc)
    if(EMCC_EXECUTABLE)
        get_filename_component(EMSCRIPTEN_ROOT_PATH "${EMCC_EXECUTABLE}" DIRECTORY)
        set(EMSCRIPTEN_ROOT_PATH "${EMSCRIPTEN_ROOT_PATH}" PARENT_SCOPE)
        message(STATUS "Found Emscripten SDK in PATH: ${EMSCRIPTEN_ROOT_PATH}")
        return()
    endif()

    message(WARNING "Emscripten SDK not found. WebAssembly builds will not be available.")
    set(EMSCRIPTEN_AVAILABLE FALSE PARENT_SCOPE)
endfunction()

# Function to configure WebAssembly target
function(configure_wasm_target target_name)
    set(options ASYNCIFY PTHREAD SIMD MODULARIZE EXPORT_ES6)
    set(oneValueArgs 
        SHELL_FILE 
        PRELOAD_FILE 
        EMBED_FILE 
        INITIAL_MEMORY 
        MAXIMUM_MEMORY 
        STACK_SIZE
        ENVIRONMENT
        EXPORT_NAME
    )
    set(multiValueArgs 
        EXPORTED_FUNCTIONS 
        EXPORTED_RUNTIME_METHODS 
        PRELOAD_FILES
        EMBED_FILES
        LINK_FLAGS
    )
    cmake_parse_arguments(WASM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        message(WARNING "configure_wasm_target called but not building for WebAssembly")
        return()
    endif()

    # Set default values
    if(NOT WASM_INITIAL_MEMORY)
        set(WASM_INITIAL_MEMORY "128MB")
    endif()

    if(NOT WASM_STACK_SIZE)
        set(WASM_STACK_SIZE "64KB")
    endif()

    if(NOT WASM_ENVIRONMENT)
        set(WASM_ENVIRONMENT "web")
    endif()

    if(NOT WASM_EXPORT_NAME)
        set(WASM_EXPORT_NAME "Module")
    endif()

    # Base WebAssembly flags
    set(WASM_FLAGS "")
    list(APPEND WASM_FLAGS "-s WASM=1")
    list(APPEND WASM_FLAGS "-s ALLOW_MEMORY_GROWTH=1")
    list(APPEND WASM_FLAGS "-s INITIAL_MEMORY=${WASM_INITIAL_MEMORY}")
    list(APPEND WASM_FLAGS "-s STACK_SIZE=${WASM_STACK_SIZE}")
    list(APPEND WASM_FLAGS "-s ENVIRONMENT=${WASM_ENVIRONMENT}")
    list(APPEND WASM_FLAGS "-s EXPORT_NAME='${WASM_EXPORT_NAME}'")

    # Optional features
    if(WASM_ASYNCIFY)
        list(APPEND WASM_FLAGS "-s ASYNCIFY=1")
        list(APPEND WASM_FLAGS "-s ASYNCIFY_STACK_SIZE=24576")
    endif()

    if(WASM_PTHREAD)
        list(APPEND WASM_FLAGS "-s USE_PTHREADS=1")
        list(APPEND WASM_FLAGS "-s PTHREAD_POOL_SIZE=4")
        target_compile_options(${target_name} PRIVATE "-pthread")
    endif()

    if(WASM_SIMD)
        list(APPEND WASM_FLAGS "-s SIMD=1")
        target_compile_options(${target_name} PRIVATE "-msimd128")
    endif()

    if(WASM_MODULARIZE)
        list(APPEND WASM_FLAGS "-s MODULARIZE=1")
    endif()

    if(WASM_EXPORT_ES6)
        list(APPEND WASM_FLAGS "-s EXPORT_ES6=1")
        list(APPEND WASM_FLAGS "-s USE_ES6_IMPORT_META=0")
    endif()

    # Maximum memory
    if(WASM_MAXIMUM_MEMORY)
        list(APPEND WASM_FLAGS "-s MAXIMUM_MEMORY=${WASM_MAXIMUM_MEMORY}")
    endif()

    # Shell file
    if(WASM_SHELL_FILE)
        if(EXISTS "${WASM_SHELL_FILE}")
            list(APPEND WASM_FLAGS "--shell-file ${WASM_SHELL_FILE}")
        else()
            message(WARNING "Shell file not found: ${WASM_SHELL_FILE}")
        endif()
    endif()

    # Preload files
    if(WASM_PRELOAD_FILES)
        foreach(PRELOAD_FILE ${WASM_PRELOAD_FILES})
            if(EXISTS "${PRELOAD_FILE}")
                list(APPEND WASM_FLAGS "--preload-file ${PRELOAD_FILE}")
            else()
                message(WARNING "Preload file not found: ${PRELOAD_FILE}")
            endif()
        endforeach()
    endif()

    # Embed files
    if(WASM_EMBED_FILES)
        foreach(EMBED_FILE ${WASM_EMBED_FILES})
            if(EXISTS "${EMBED_FILE}")
                list(APPEND WASM_FLAGS "--embed-file ${EMBED_FILE}")
            else()
                message(WARNING "Embed file not found: ${EMBED_FILE}")
            endif()
        endforeach()
    endif()

    # Exported functions
    if(WASM_EXPORTED_FUNCTIONS)
        string(REPLACE ";" "," EXPORTED_FUNCS "${WASM_EXPORTED_FUNCTIONS}")
        list(APPEND WASM_FLAGS "-s EXPORTED_FUNCTIONS=[${EXPORTED_FUNCS}]")
    endif()

    # Exported runtime methods
    if(WASM_EXPORTED_RUNTIME_METHODS)
        string(REPLACE ";" "," EXPORTED_METHODS "${WASM_EXPORTED_RUNTIME_METHODS}")
        list(APPEND WASM_FLAGS "-s EXPORTED_RUNTIME_METHODS=[${EXPORTED_METHODS}]")
    endif()

    # Additional link flags
    if(WASM_LINK_FLAGS)
        list(APPEND WASM_FLAGS ${WASM_LINK_FLAGS})
    endif()

    # Apply flags to target
    target_link_options(${target_name} PRIVATE ${WASM_FLAGS})

    # Set output file extensions
    set_target_properties(${target_name} PROPERTIES
        SUFFIX ".js"
    )

    message(STATUS "Configured WebAssembly target: ${target_name}")
endfunction()

# Function to add WebAssembly optimization flags
function(add_wasm_optimizations target_name)
    set(options SIZE_OPTIMIZED SPEED_OPTIMIZED DEBUG_OPTIMIZED)
    set(oneValueArgs OPTIMIZATION_LEVEL)
    set(multiValueArgs)
    cmake_parse_arguments(OPT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        return()
    endif()

    if(OPT_SIZE_OPTIMIZED)
        target_compile_options(${target_name} PRIVATE "-Oz")
        target_link_options(${target_name} PRIVATE 
            "-s AGGRESSIVE_VARIABLE_ELIMINATION=1"
            "-s ELIMINATE_DUPLICATE_FUNCTIONS=1"
            "--closure 1"
        )
    elseif(OPT_SPEED_OPTIMIZED)
        target_compile_options(${target_name} PRIVATE "-O3")
        target_link_options(${target_name} PRIVATE 
            "-s AGGRESSIVE_VARIABLE_ELIMINATION=1"
            "-s ELIMINATE_DUPLICATE_FUNCTIONS=1"
        )
    elseif(OPT_DEBUG_OPTIMIZED)
        target_compile_options(${target_name} PRIVATE "-O0" "-g")
        target_link_options(${target_name} PRIVATE 
            "-s ASSERTIONS=1"
            "-s SAFE_HEAP=1"
            "-s STACK_OVERFLOW_CHECK=1"
        )
    endif()

    if(OPT_OPTIMIZATION_LEVEL)
        target_compile_options(${target_name} PRIVATE "-O${OPT_OPTIMIZATION_LEVEL}")
    endif()
endfunction()

# Function to create WebAssembly library
function(create_wasm_library target_name)
    set(options SIDE_MODULE)
    set(oneValueArgs)
    set(multiValueArgs SOURCES EXPORTED_FUNCTIONS)
    cmake_parse_arguments(LIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        message(WARNING "create_wasm_library called but not building for WebAssembly")
        return()
    endif()

    add_library(${target_name} ${LIB_SOURCES})

    if(LIB_SIDE_MODULE)
        target_link_options(${target_name} PRIVATE "-s SIDE_MODULE=1")
        set_target_properties(${target_name} PROPERTIES
            SUFFIX ".wasm"
        )
    endif()

    if(LIB_EXPORTED_FUNCTIONS)
        string(REPLACE ";" "," EXPORTED_FUNCS "${LIB_EXPORTED_FUNCTIONS}")
        target_link_options(${target_name} PRIVATE 
            "-s EXPORTED_FUNCTIONS=[${EXPORTED_FUNCS}]"
        )
    endif()

    message(STATUS "Created WebAssembly library: ${target_name}")
endfunction()

# Function to setup WebAssembly development environment
function(setup_wasm_environment)
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        return()
    endif()

    detect_emscripten_sdk()

    if(NOT EMSCRIPTEN_AVAILABLE)
        return()
    endif()

    # Set common WebAssembly definitions
    add_definitions(-DEMSCRIPTEN)
    add_definitions(-D__EMSCRIPTEN__)

    # Configure common settings
    set(CMAKE_EXECUTABLE_SUFFIX ".js" PARENT_SCOPE)
    set(BUILD_SHARED_LIBS OFF PARENT_SCOPE)

    message(STATUS "WebAssembly development environment configured")
endfunction()

# Function to validate WebAssembly build
function(validate_wasm_build target_name)
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        return()
    endif()

    # Add custom command to validate the generated WebAssembly
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Validating WebAssembly build: ${target_name}"
        COMMAND node --version > /dev/null 2>&1 || echo "Warning: Node.js not found, cannot test WebAssembly module"
        COMMENT "Validating WebAssembly build"
    )
endfunction()

# Function to create WebAssembly test
function(create_wasm_test test_name target_name)
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
        return()
    endif()

    # Check if Node.js is available
    find_program(NODE_EXECUTABLE node)
    if(NOT NODE_EXECUTABLE)
        message(WARNING "Node.js not found. WebAssembly tests will not be available.")
        return()
    endif()

    add_test(NAME ${test_name}
        COMMAND ${NODE_EXECUTABLE} $<TARGET_FILE:${target_name}>
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    set_tests_properties(${test_name} PROPERTIES
        ENVIRONMENT "NODE_PATH=${CMAKE_CURRENT_BINARY_DIR}"
    )

    message(STATUS "Created WebAssembly test: ${test_name}")
endfunction()
