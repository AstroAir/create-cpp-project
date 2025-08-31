# ContainerUtils.cmake
# Utilities for managing Docker containers in the build system

# Function to check if Docker is available
function(check_docker_availability)
    find_program(DOCKER_EXECUTABLE docker)
    
    if(NOT DOCKER_EXECUTABLE)
        message(WARNING "Docker not found. Container-based builds will not be available.")
        set(DOCKER_AVAILABLE FALSE PARENT_SCOPE)
        return()
    endif()
    
    # Check if Docker daemon is running
    execute_process(
        COMMAND ${DOCKER_EXECUTABLE} info
        OUTPUT_QUIET
        ERROR_QUIET
        RESULT_VARIABLE DOCKER_RESULT
    )
    
    if(DOCKER_RESULT EQUAL 0)
        set(DOCKER_AVAILABLE TRUE PARENT_SCOPE)
        message(STATUS "Docker is available and running")
    else()
        set(DOCKER_AVAILABLE FALSE PARENT_SCOPE)
        message(WARNING "Docker is installed but not running. Container-based builds will not be available.")
    endif()
endfunction()

# Function to check if Docker Compose is available
function(check_docker_compose_availability)
    find_program(DOCKER_COMPOSE_EXECUTABLE docker-compose)
    
    if(NOT DOCKER_COMPOSE_EXECUTABLE)
        # Try docker compose (newer syntax)
        execute_process(
            COMMAND docker compose version
            OUTPUT_QUIET
            ERROR_QUIET
            RESULT_VARIABLE COMPOSE_RESULT
        )
        
        if(COMPOSE_RESULT EQUAL 0)
            set(DOCKER_COMPOSE_EXECUTABLE "docker compose" PARENT_SCOPE)
            set(DOCKER_COMPOSE_AVAILABLE TRUE PARENT_SCOPE)
            message(STATUS "Docker Compose (plugin) is available")
        else()
            set(DOCKER_COMPOSE_AVAILABLE FALSE PARENT_SCOPE)
            message(WARNING "Docker Compose not found. Multi-container builds will not be available.")
        endif()
    else()
        set(DOCKER_COMPOSE_AVAILABLE TRUE PARENT_SCOPE)
        message(STATUS "Docker Compose is available")
    endif()
endfunction()

# Function to build container image
function(build_container_image)
    set(options FORCE_REBUILD NO_CACHE)
    set(oneValueArgs IMAGE_NAME DOCKERFILE_PATH CONTEXT_PATH TAG)
    set(multiValueArgs BUILD_ARGS)
    cmake_parse_arguments(BUILD "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT BUILD_IMAGE_NAME)
        message(FATAL_ERROR "IMAGE_NAME is required for build_container_image")
    endif()

    if(NOT BUILD_DOCKERFILE_PATH)
        set(BUILD_DOCKERFILE_PATH "${CMAKE_SOURCE_DIR}/docker/${BUILD_IMAGE_NAME}.Dockerfile")
    endif()

    if(NOT BUILD_CONTEXT_PATH)
        set(BUILD_CONTEXT_PATH "${CMAKE_SOURCE_DIR}")
    endif()

    if(NOT BUILD_TAG)
        set(BUILD_TAG "latest")
    endif()

    set(FULL_IMAGE_NAME "${BUILD_IMAGE_NAME}:${BUILD_TAG}")

    # Check if image exists and if rebuild is needed
    if(NOT BUILD_FORCE_REBUILD)
        execute_process(
            COMMAND ${DOCKER_EXECUTABLE} images -q ${FULL_IMAGE_NAME}
            OUTPUT_VARIABLE IMAGE_ID
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        
        if(IMAGE_ID)
            message(STATUS "Container image ${FULL_IMAGE_NAME} already exists")
            return()
        endif()
    endif()

    message(STATUS "Building container image: ${FULL_IMAGE_NAME}")

    # Prepare build command
    set(BUILD_CMD ${DOCKER_EXECUTABLE} build)
    list(APPEND BUILD_CMD -f "${BUILD_DOCKERFILE_PATH}")
    list(APPEND BUILD_CMD -t "${FULL_IMAGE_NAME}")

    if(BUILD_NO_CACHE)
        list(APPEND BUILD_CMD --no-cache)
    endif()

    # Add build arguments
    foreach(BUILD_ARG ${BUILD_BUILD_ARGS})
        list(APPEND BUILD_CMD --build-arg "${BUILD_ARG}")
    endforeach()

    list(APPEND BUILD_CMD "${BUILD_CONTEXT_PATH}")

    # Execute build command
    execute_process(
        COMMAND ${BUILD_CMD}
        RESULT_VARIABLE BUILD_RESULT
        OUTPUT_VARIABLE BUILD_OUTPUT
        ERROR_VARIABLE BUILD_ERROR
    )

    if(BUILD_RESULT EQUAL 0)
        message(STATUS "Successfully built container image: ${FULL_IMAGE_NAME}")
    else()
        message(FATAL_ERROR "Failed to build container image: ${FULL_IMAGE_NAME}\nError: ${BUILD_ERROR}")
    endif()
endfunction()

# Function to run command in container
function(run_in_container)
    set(options INTERACTIVE REMOVE_AFTER)
    set(oneValueArgs IMAGE_NAME WORKING_DIR COMMAND)
    set(multiValueArgs VOLUMES ENVIRONMENT_VARS EXTRA_ARGS)
    cmake_parse_arguments(RUN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT RUN_IMAGE_NAME)
        message(FATAL_ERROR "IMAGE_NAME is required for run_in_container")
    endif()

    if(NOT RUN_COMMAND)
        set(RUN_COMMAND "/bin/bash")
    endif()

    if(NOT RUN_WORKING_DIR)
        set(RUN_WORKING_DIR "/workspace")
    endif()

    # Prepare run command
    set(RUN_CMD ${DOCKER_EXECUTABLE} run)

    if(RUN_REMOVE_AFTER)
        list(APPEND RUN_CMD --rm)
    endif()

    if(RUN_INTERACTIVE)
        list(APPEND RUN_CMD -it)
    endif()

    list(APPEND RUN_CMD -w "${RUN_WORKING_DIR}")

    # Add volumes
    foreach(VOLUME ${RUN_VOLUMES})
        list(APPEND RUN_CMD -v "${VOLUME}")
    endforeach()

    # Add environment variables
    foreach(ENV_VAR ${RUN_ENVIRONMENT_VARS})
        list(APPEND RUN_CMD -e "${ENV_VAR}")
    endforeach()

    # Add extra arguments
    foreach(EXTRA_ARG ${RUN_EXTRA_ARGS})
        list(APPEND RUN_CMD "${EXTRA_ARG}")
    endforeach()

    list(APPEND RUN_CMD "${RUN_IMAGE_NAME}")
    list(APPEND RUN_CMD ${RUN_COMMAND})

    message(STATUS "Running command in container: ${RUN_IMAGE_NAME}")
    message(STATUS "Command: ${RUN_CMD}")

    # Execute run command
    execute_process(
        COMMAND ${RUN_CMD}
        RESULT_VARIABLE RUN_RESULT
    )

    if(NOT RUN_RESULT EQUAL 0)
        message(WARNING "Container command failed with exit code: ${RUN_RESULT}")
    endif()
endfunction()

# Function to validate container image
function(validate_container_image)
    set(options VERBOSE)
    set(oneValueArgs IMAGE_NAME)
    set(multiValueArgs REQUIRED_TOOLS)
    cmake_parse_arguments(VALIDATE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT VALIDATE_IMAGE_NAME)
        message(FATAL_ERROR "IMAGE_NAME is required for validate_container_image")
    endif()

    message(STATUS "Validating container image: ${VALIDATE_IMAGE_NAME}")

    # Check if image exists
    execute_process(
        COMMAND ${DOCKER_EXECUTABLE} images -q ${VALIDATE_IMAGE_NAME}
        OUTPUT_VARIABLE IMAGE_ID
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT IMAGE_ID)
        message(FATAL_ERROR "Container image not found: ${VALIDATE_IMAGE_NAME}")
    endif()

    # Validate required tools
    if(VALIDATE_REQUIRED_TOOLS)
        foreach(TOOL ${VALIDATE_REQUIRED_TOOLS})
            execute_process(
                COMMAND ${DOCKER_EXECUTABLE} run --rm ${VALIDATE_IMAGE_NAME} which ${TOOL}
                OUTPUT_QUIET
                ERROR_QUIET
                RESULT_VARIABLE TOOL_RESULT
            )

            if(TOOL_RESULT EQUAL 0)
                if(VALIDATE_VERBOSE)
                    message(STATUS "  ✓ ${TOOL} is available")
                endif()
            else()
                message(FATAL_ERROR "Required tool '${TOOL}' not found in container: ${VALIDATE_IMAGE_NAME}")
            endif()
        endforeach()
    endif()

    message(STATUS "Container image validation passed: ${VALIDATE_IMAGE_NAME}")
endfunction()

# Function to clean up container images
function(cleanup_container_images)
    set(options FORCE REMOVE_DANGLING REMOVE_ALL)
    set(oneValueArgs IMAGE_PREFIX)
    set(multiValueArgs)
    cmake_parse_arguments(CLEANUP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    message(STATUS "Cleaning up container images...")

    if(CLEANUP_REMOVE_DANGLING)
        execute_process(
            COMMAND ${DOCKER_EXECUTABLE} image prune -f
            OUTPUT_VARIABLE CLEANUP_OUTPUT
        )
        message(STATUS "Removed dangling images")
    endif()

    if(CLEANUP_IMAGE_PREFIX)
        execute_process(
            COMMAND ${DOCKER_EXECUTABLE} images --format "{{.Repository}}:{{.Tag}}" --filter "reference=${CLEANUP_IMAGE_PREFIX}*"
            OUTPUT_VARIABLE IMAGES_TO_REMOVE
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        if(IMAGES_TO_REMOVE)
            string(REPLACE "\n" ";" IMAGE_LIST "${IMAGES_TO_REMOVE}")
            foreach(IMAGE ${IMAGE_LIST})
                if(CLEANUP_FORCE)
                    execute_process(
                        COMMAND ${DOCKER_EXECUTABLE} rmi -f ${IMAGE}
                        OUTPUT_QUIET
                        ERROR_QUIET
                    )
                else()
                    execute_process(
                        COMMAND ${DOCKER_EXECUTABLE} rmi ${IMAGE}
                        OUTPUT_QUIET
                        ERROR_QUIET
                    )
                endif()
                message(STATUS "Removed image: ${IMAGE}")
            endforeach()
        endif()
    endif()

    if(CLEANUP_REMOVE_ALL)
        if(CLEANUP_FORCE)
            execute_process(
                COMMAND ${DOCKER_EXECUTABLE} system prune -af
                OUTPUT_VARIABLE CLEANUP_OUTPUT
            )
        else()
            execute_process(
                COMMAND ${DOCKER_EXECUTABLE} system prune -a
                OUTPUT_VARIABLE CLEANUP_OUTPUT
            )
        endif()
        message(STATUS "Removed all unused images and containers")
    endif()
endfunction()

# Function to setup container build environment
function(setup_container_build_environment)
    check_docker_availability()
    check_docker_compose_availability()

    if(NOT DOCKER_AVAILABLE)
        message(WARNING "Docker is not available. Container builds will be disabled.")
        set(ENABLE_CONTAINER_BUILDS FALSE PARENT_SCOPE)
        return()
    endif()

    set(ENABLE_CONTAINER_BUILDS TRUE PARENT_SCOPE)
    message(STATUS "Container build environment is ready")

    # Set container-related variables
    set(CONTAINER_REGISTRY "cpp-scaffold" PARENT_SCOPE)
    set(CONTAINER_TAG "latest" PARENT_SCOPE)
endfunction()
