# PlatformTesting.cmake
# Comprehensive platform testing matrix for all supported platforms

# Function to create platform testing matrix
function(create_platform_testing_matrix)
    set(options VERBOSE PARALLEL)
    set(oneValueArgs TEST_TIMEOUT)
    set(multiValueArgs PLATFORMS CONFIGURATIONS ARCHITECTURES)
    cmake_parse_arguments(MATRIX "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT MATRIX_PLATFORMS)
        set(MATRIX_PLATFORMS 
            "windows" "linux" "macos" 
            "android" "ios" 
            "wasm" "embedded"
        )
    endif()

    if(NOT MATRIX_CONFIGURATIONS)
        set(MATRIX_CONFIGURATIONS "Debug" "Release")
    endif()

    if(NOT MATRIX_ARCHITECTURES)
        set(MATRIX_ARCHITECTURES "x64" "x86" "arm64" "arm")
    endif()

    if(NOT MATRIX_TEST_TIMEOUT)
        set(MATRIX_TEST_TIMEOUT 300)  # 5 minutes
    endif()

    message(STATUS "Creating platform testing matrix...")

    # Create master test target
    add_custom_target(test-all-platforms
        COMMENT "Running tests on all supported platforms"
    )

    # Create platform-specific test targets
    foreach(PLATFORM ${MATRIX_PLATFORMS})
        foreach(CONFIG ${MATRIX_CONFIGURATIONS})
            foreach(ARCH ${MATRIX_ARCHITECTURES})
                set(TEST_NAME "test-${PLATFORM}-${CONFIG}-${ARCH}")
                
                # Check if this combination is valid
                if(is_valid_platform_combination(${PLATFORM} ${CONFIG} ${ARCH}))
                    create_platform_test_target(${TEST_NAME} ${PLATFORM} ${CONFIG} ${ARCH})
                    add_dependencies(test-all-platforms ${TEST_NAME})
                    
                    if(MATRIX_VERBOSE)
                        message(STATUS "Added test target: ${TEST_NAME}")
                    endif()
                endif()
            endforeach()
        endforeach()
    endforeach()

    message(STATUS "Platform testing matrix created")
endfunction()

# Function to check if platform combination is valid
function(is_valid_platform_combination platform config arch result_var)
    set(${result_var} TRUE PARENT_SCOPE)

    # Platform-specific architecture validation
    if(platform STREQUAL "ios" AND NOT arch MATCHES "arm64|x64")
        set(${result_var} FALSE PARENT_SCOPE)
    elseif(platform STREQUAL "android" AND arch STREQUAL "x86")
        # x86 Android is less common, skip for now
        set(${result_var} FALSE PARENT_SCOPE)
    elseif(platform STREQUAL "embedded" AND NOT arch MATCHES "arm")
        set(${result_var} FALSE PARENT_SCOPE)
    elseif(platform STREQUAL "wasm" AND NOT arch STREQUAL "wasm32")
        set(${result_var} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Function to create platform-specific test target
function(create_platform_test_target test_name platform config arch)
    if(platform STREQUAL "windows")
        create_windows_test_target(${test_name} ${config} ${arch})
    elseif(platform STREQUAL "linux")
        create_linux_test_target(${test_name} ${config} ${arch})
    elseif(platform STREQUAL "macos")
        create_macos_test_target(${test_name} ${config} ${arch})
    elseif(platform STREQUAL "android")
        create_android_test_target(${test_name} ${config} ${arch})
    elseif(platform STREQUAL "ios")
        create_ios_test_target(${test_name} ${config} ${arch})
    elseif(platform STREQUAL "wasm")
        create_wasm_test_target(${test_name} ${config})
    elseif(platform STREQUAL "embedded")
        create_embedded_test_target(${test_name} ${config} ${arch})
    endif()
endfunction()

# Function to create Windows test target
function(create_windows_test_target test_name config arch)
    set(PRESET_NAME "windows-${arch}-${config}")
    
    add_custom_target(${test_name}
        COMMAND ${CMAKE_COMMAND} --build . --preset ${PRESET_NAME}
        COMMAND ${CMAKE_CTEST_COMMAND} --preset ${PRESET_NAME}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Testing Windows ${arch} ${config}"
    )
endfunction()

# Function to create Linux test target
function(create_linux_test_target test_name config arch)
    set(PRESET_NAME "linux-${arch}-${config}")
    
    add_custom_target(${test_name}
        COMMAND ${CMAKE_COMMAND} --build . --preset ${PRESET_NAME}
        COMMAND ${CMAKE_CTEST_COMMAND} --preset ${PRESET_NAME}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Testing Linux ${arch} ${config}"
    )
endfunction()

# Function to create macOS test target
function(create_macos_test_target test_name config arch)
    set(PRESET_NAME "macos-${arch}-${config}")
    
    add_custom_target(${test_name}
        COMMAND ${CMAKE_COMMAND} --build . --preset ${PRESET_NAME}
        COMMAND ${CMAKE_CTEST_COMMAND} --preset ${PRESET_NAME}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Testing macOS ${arch} ${config}"
    )
endfunction()

# Function to create Android test target
function(create_android_test_target test_name config arch)
    set(PRESET_NAME "android-${arch}-${config}")
    
    # Check if Android emulator is available
    find_program(ADB_EXECUTABLE adb)
    if(ADB_EXECUTABLE)
        add_custom_target(${test_name}
            COMMAND ${CMAKE_COMMAND} --build . --preset ${PRESET_NAME}
            COMMAND ${CMAKE_COMMAND} -E echo "Deploying to Android device/emulator..."
            COMMAND ${ADB_EXECUTABLE} devices
            COMMAND ${CMAKE_CTEST_COMMAND} --preset ${PRESET_NAME}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Testing Android ${arch} ${config}"
        )
    else()
        add_custom_target(${test_name}
            COMMAND ${CMAKE_COMMAND} --build . --preset ${PRESET_NAME}
            COMMAND ${CMAKE_COMMAND} -E echo "ADB not found, skipping device tests"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Building Android ${arch} ${config} (no device testing)"
        )
    endif()
endfunction()

# Function to create iOS test target
function(create_ios_test_target test_name config arch)
    if(NOT APPLE)
        return()
    endif()

    set(PRESET_NAME "ios-${arch}-${config}")
    
    # Check if iOS simulator is available
    find_program(XCRUN_EXECUTABLE xcrun)
    if(XCRUN_EXECUTABLE)
        add_custom_target(${test_name}
            COMMAND ${CMAKE_COMMAND} --build . --preset ${PRESET_NAME}
            COMMAND ${XCRUN_EXECUTABLE} simctl list devices
            COMMAND ${CMAKE_CTEST_COMMAND} --preset ${PRESET_NAME}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Testing iOS ${arch} ${config}"
        )
    else()
        add_custom_target(${test_name}
            COMMAND ${CMAKE_COMMAND} --build . --preset ${PRESET_NAME}
            COMMAND ${CMAKE_COMMAND} -E echo "xcrun not found, skipping simulator tests"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Building iOS ${arch} ${config} (no simulator testing)"
        )
    endif()
endfunction()

# Function to create WebAssembly test target
function(create_wasm_test_target test_name config)
    set(PRESET_NAME "wasm-${config}")
    
    # Check if Node.js is available for testing
    find_program(NODE_EXECUTABLE node)
    if(NODE_EXECUTABLE)
        add_custom_target(${test_name}
            COMMAND ${CMAKE_COMMAND} --build . --preset ${PRESET_NAME}
            COMMAND ${NODE_EXECUTABLE} --version
            COMMAND ${CMAKE_CTEST_COMMAND} --preset ${PRESET_NAME}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Testing WebAssembly ${config}"
        )
    else()
        add_custom_target(${test_name}
            COMMAND ${CMAKE_COMMAND} --build . --preset ${PRESET_NAME}
            COMMAND ${CMAKE_COMMAND} -E echo "Node.js not found, skipping WebAssembly tests"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Building WebAssembly ${config} (no runtime testing)"
        )
    endif()
endfunction()

# Function to create embedded test target
function(create_embedded_test_target test_name config arch)
    set(PRESET_NAME "embedded-${arch}-${config}")
    
    # Check if QEMU is available for testing
    find_program(QEMU_ARM_EXECUTABLE qemu-arm)
    if(QEMU_ARM_EXECUTABLE)
        add_custom_target(${test_name}
            COMMAND ${CMAKE_COMMAND} --build . --preset ${PRESET_NAME}
            COMMAND ${QEMU_ARM_EXECUTABLE} --version
            COMMAND ${CMAKE_CTEST_COMMAND} --preset ${PRESET_NAME}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Testing Embedded ${arch} ${config}"
        )
    else()
        add_custom_target(${test_name}
            COMMAND ${CMAKE_COMMAND} --build . --preset ${PRESET_NAME}
            COMMAND ${CMAKE_COMMAND} -E echo "QEMU not found, skipping embedded tests"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Building Embedded ${arch} ${config} (no emulation testing)"
        )
    endif()
endfunction()

# Function to create container-based testing matrix
function(create_container_testing_matrix)
    set(options VERBOSE)
    set(oneValueArgs)
    set(multiValueArgs CONTAINERS CONFIGURATIONS)
    cmake_parse_arguments(CONTAINER "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT CONTAINER_CONTAINERS)
        set(CONTAINER_CONTAINERS 
            "ubuntu-dev" "alpine-minimal" 
            "android-dev" "wasm-dev" "ci-builder"
        )
    endif()

    if(NOT CONTAINER_CONFIGURATIONS)
        set(CONTAINER_CONFIGURATIONS "Debug" "Release")
    endif()

    # Check if Docker is available
    find_program(DOCKER_EXECUTABLE docker)
    if(NOT DOCKER_EXECUTABLE)
        message(WARNING "Docker not found. Container tests will not be available.")
        return()
    endif()

    message(STATUS "Creating container testing matrix...")

    # Create master container test target
    add_custom_target(test-all-containers
        COMMENT "Running tests in all containers"
    )

    foreach(CONTAINER ${CONTAINER_CONTAINERS})
        foreach(CONFIG ${CONTAINER_CONFIGURATIONS})
            set(TEST_NAME "test-container-${CONTAINER}-${CONFIG}")
            
            add_custom_target(${TEST_NAME}
                COMMAND ${CMAKE_COMMAND} -E echo "Testing in container: ${CONTAINER} (${CONFIG})"
                COMMAND ${CMAKE_SOURCE_DIR}/scripts/manage-containers.sh run ${CONTAINER} 
                        build --preset ${CONFIG}
                COMMAND ${CMAKE_SOURCE_DIR}/scripts/manage-containers.sh run ${CONTAINER} 
                        test --preset ${CONFIG}
                COMMENT "Testing in ${CONTAINER} container (${CONFIG})"
            )
            
            add_dependencies(test-all-containers ${TEST_NAME})
            
            if(CONTAINER_VERBOSE)
                message(STATUS "Added container test target: ${TEST_NAME}")
            endif()
        endforeach()
    endforeach()

    message(STATUS "Container testing matrix created")
endfunction()

# Function to create performance testing matrix
function(create_performance_testing_matrix)
    set(options VERBOSE)
    set(oneValueArgs BENCHMARK_TIMEOUT)
    set(multiValueArgs PLATFORMS BENCHMARKS)
    cmake_parse_arguments(PERF "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT PERF_PLATFORMS)
        set(PERF_PLATFORMS "windows" "linux" "macos")
    endif()

    if(NOT PERF_BENCHMARKS)
        set(PERF_BENCHMARKS "build-time" "runtime" "memory")
    endif()

    if(NOT PERF_BENCHMARK_TIMEOUT)
        set(PERF_BENCHMARK_TIMEOUT 600)  # 10 minutes
    endif()

    message(STATUS "Creating performance testing matrix...")

    # Create master performance test target
    add_custom_target(test-performance
        COMMENT "Running performance tests on all platforms"
    )

    foreach(PLATFORM ${PERF_PLATFORMS})
        foreach(BENCHMARK ${PERF_BENCHMARKS})
            set(TEST_NAME "perf-${PLATFORM}-${BENCHMARK}")
            
            add_custom_target(${TEST_NAME}
                COMMAND ${CMAKE_COMMAND} -E echo "Running ${BENCHMARK} benchmark on ${PLATFORM}"
                COMMAND ${CMAKE_CTEST_COMMAND} -R "benchmark_${BENCHMARK}" --timeout ${PERF_BENCHMARK_TIMEOUT}
                COMMENT "Performance test: ${PLATFORM} ${BENCHMARK}"
            )
            
            add_dependencies(test-performance ${TEST_NAME})
            
            if(PERF_VERBOSE)
                message(STATUS "Added performance test target: ${TEST_NAME}")
            endif()
        endforeach()
    endforeach()

    message(STATUS "Performance testing matrix created")
endfunction()

# Function to setup comprehensive testing
function(setup_comprehensive_testing)
    message(STATUS "Setting up comprehensive testing matrix...")
    
    # Create all testing matrices
    create_platform_testing_matrix(VERBOSE)
    create_container_testing_matrix(VERBOSE)
    create_performance_testing_matrix(VERBOSE)
    
    # Create master test target
    add_custom_target(test-comprehensive
        COMMENT "Running comprehensive test suite"
    )
    
    add_dependencies(test-comprehensive 
        test-all-platforms 
        test-all-containers 
        test-performance
    )
    
    message(STATUS "Comprehensive testing matrix configured")
endfunction()
