# Dependencies.cmake
# Third-party library configuration and dependency management

# Configure FetchContent for better performance
function(configure_fetchcontent)
    include(FetchContent)

    # Set global cache directory for dependencies
    set(DEPS_CACHE_DIR "${CMAKE_SOURCE_DIR}/deps_cache" CACHE PATH "Dependency cache directory")
    file(MAKE_DIRECTORY ${DEPS_CACHE_DIR})

    # Configure FetchContent for better performance
    set(FETCHCONTENT_QUIET OFF PARENT_SCOPE)
    set(FETCHCONTENT_UPDATES_DISCONNECTED ON PARENT_SCOPE)
endfunction()

# Custom function for caching downloaded dependencies
function(fetch_and_cache NAME GIT_REPO GIT_TAG)
    # Create cache directory path, replacing all special characters
    string(REPLACE "/" "_" SAFE_REPO_NAME ${GIT_REPO})
    string(REPLACE ":" "_" SAFE_REPO_NAME ${SAFE_REPO_NAME})
    string(REPLACE "." "_" SAFE_REPO_NAME ${SAFE_REPO_NAME})
    set(CACHE_DIR "${DEPS_CACHE_DIR}/${NAME}_${SAFE_REPO_NAME}_${GIT_TAG}")

    # Check if cache directory exists
    if(EXISTS ${CACHE_DIR})
        message(STATUS "Using cached dependency: ${NAME} (${GIT_TAG})")
        FetchContent_Declare(
            ${NAME}
            SOURCE_DIR ${CACHE_DIR}
        )
    else()
        message(STATUS "Downloading dependency: ${NAME} (${GIT_TAG})")
        FetchContent_Declare(
            ${NAME}
            GIT_REPOSITORY ${GIT_REPO}
            GIT_TAG ${GIT_TAG}
            SOURCE_DIR ${CACHE_DIR}
        )
    endif()

    # Make dependency available
    FetchContent_MakeAvailable(${NAME})
endfunction()

# Enhanced dependency fetching with version management
function(fetch_dependency NAME)
    set(options REQUIRED QUIET)
    set(oneValueArgs GIT_REPOSITORY GIT_TAG FIND_PACKAGE_NAME VERSION)
    set(multiValueArgs CMAKE_ARGS)
    cmake_parse_arguments(FETCH_DEP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Set default find package name if not provided
    if(NOT FETCH_DEP_FIND_PACKAGE_NAME)
        set(FETCH_DEP_FIND_PACKAGE_NAME ${NAME})
    endif()

    # Try to find system package first
    if(FETCH_DEP_QUIET)
        find_package(${FETCH_DEP_FIND_PACKAGE_NAME} ${FETCH_DEP_VERSION} QUIET)
    else()
        find_package(${FETCH_DEP_FIND_PACKAGE_NAME} ${FETCH_DEP_VERSION})
    endif()

    # Use system package if found, otherwise fetch
    if(${FETCH_DEP_FIND_PACKAGE_NAME}_FOUND)
        message(STATUS "Using system ${FETCH_DEP_FIND_PACKAGE_NAME}: ${${FETCH_DEP_FIND_PACKAGE_NAME}_VERSION}")
    else()
        if(FETCH_DEP_REQUIRED AND NOT FETCH_DEP_GIT_REPOSITORY)
            message(FATAL_ERROR "System ${FETCH_DEP_FIND_PACKAGE_NAME} not found and no Git repository provided")
        elseif(FETCH_DEP_GIT_REPOSITORY)
            message(STATUS "System ${FETCH_DEP_FIND_PACKAGE_NAME} not found, will use fetched version")
            fetch_and_cache(${NAME} ${FETCH_DEP_GIT_REPOSITORY} ${FETCH_DEP_GIT_TAG})
        endif()
    endif()
endfunction()

# Setup Core Dependencies
function(setup_core_dependencies)
    configure_fetchcontent()

    # spdlog - Logging library
    fetch_dependency(spdlog
        FIND_PACKAGE_NAME spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
        QUIET
    )

    # nlohmann_json - JSON library
    fetch_dependency(json
        FIND_PACKAGE_NAME nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        QUIET
    )
endfunction()

# Setup Test Dependencies
function(setup_test_dependencies)
    if(BUILD_TESTING)
        # Google Test with caching
        fetch_and_cache(
            googletest
            https://github.com/google/googletest.git
            v1.14.0
        )
    endif()
endfunction()

# Setup Optional Dependencies
function(setup_optional_dependencies)
    # Add optional dependencies here
    # Example:
    # option(ENABLE_CURL "Enable HTTP client support" OFF)
    # if(ENABLE_CURL)
    #     fetch_dependency(curl
    #         FIND_PACKAGE_NAME CURL
    #         GIT_REPOSITORY https://github.com/curl/curl.git
    #         GIT_TAG curl-8_4_0
    #     )
    # endif()
endfunction()

# Filesystem Library Detection
function(setup_filesystem_library target)
    # Detect filesystem library requirements
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
            find_package(Filesystem REQUIRED)
            if(NOT Filesystem_FOUND)
                message(FATAL_ERROR "Filesystem library not found. Use a newer compiler or provide stdc++fs library.")
            endif()
            target_link_libraries(${target} PRIVATE std::filesystem)
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
            find_package(Filesystem REQUIRED)
            if(NOT Filesystem_FOUND)
                message(FATAL_ERROR "Filesystem library not found. Use a newer compiler or provide c++fs library.")
            endif()
            target_link_libraries(${target} PRIVATE std::filesystem)
        endif()
    endif()
endfunction()

# Platform-specific Libraries
function(setup_platform_libraries target)
    if(WIN32)
        target_link_libraries(${target} PRIVATE shlwapi)
    endif()
endfunction()

# Link Core Dependencies to Target
function(link_core_dependencies target)
    target_link_libraries(${target} PRIVATE
        spdlog::spdlog
        nlohmann_json::nlohmann_json
    )

    setup_filesystem_library(${target})
    setup_platform_libraries(${target})
endfunction()

# Setup All Dependencies
function(setup_all_dependencies)
    include(PackageManager)
    setup_enhanced_package_management()
    setup_optional_dependencies()
endfunction()
