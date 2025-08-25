# PackageManager.cmake
# Enhanced package management with version control and multiple package managers

# Package manager detection and configuration
function(detect_package_managers)
    # vcpkg detection
    if(DEFINED ENV{VCPKG_ROOT})
        set(VCPKG_ROOT $ENV{VCPKG_ROOT} PARENT_SCOPE)
        set(VCPKG_AVAILABLE TRUE PARENT_SCOPE)
        message(STATUS "vcpkg detected at: $ENV{VCPKG_ROOT}")
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/vcpkg")
        set(VCPKG_ROOT "${CMAKE_SOURCE_DIR}/vcpkg" PARENT_SCOPE)
        set(VCPKG_AVAILABLE TRUE PARENT_SCOPE)
        message(STATUS "vcpkg detected at: ${CMAKE_SOURCE_DIR}/vcpkg")
    endif()

    # Conan detection
    find_program(CONAN_EXECUTABLE conan)
    if(CONAN_EXECUTABLE)
        set(CONAN_AVAILABLE TRUE PARENT_SCOPE)
        message(STATUS "Conan detected at: ${CONAN_EXECUTABLE}")
    endif()

    # Hunter detection
    if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/HunterGate.cmake")
        set(HUNTER_AVAILABLE TRUE PARENT_SCOPE)
        message(STATUS "Hunter detected")
    endif()

    # CPM detection
    if(EXISTS "${CMAKE_SOURCE_DIR}/cmake/CPM.cmake")
        set(CPM_AVAILABLE TRUE PARENT_SCOPE)
        message(STATUS "CPM detected")
    endif()
endfunction()

# Configure vcpkg integration
function(setup_vcpkg)
    if(VCPKG_AVAILABLE AND VCPKG_ROOT)
        set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" PARENT_SCOPE)
        message(STATUS "vcpkg toolchain configured")

        # Set vcpkg triplet based on platform
        if(NOT VCPKG_TARGET_TRIPLET)
            if(WIN32)
                if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                    set(VCPKG_TARGET_TRIPLET "x64-windows" PARENT_SCOPE)
                else()
                    set(VCPKG_TARGET_TRIPLET "x86-windows" PARENT_SCOPE)
                endif()
            elseif(APPLE)
                if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
                    set(VCPKG_TARGET_TRIPLET "arm64-osx" PARENT_SCOPE)
                else()
                    set(VCPKG_TARGET_TRIPLET "x64-osx" PARENT_SCOPE)
                endif()
            else()
                if(CMAKE_SIZEOF_VOID_P EQUAL 8)
                    set(VCPKG_TARGET_TRIPLET "x64-linux" PARENT_SCOPE)
                else()
                    set(VCPKG_TARGET_TRIPLET "x86-linux" PARENT_SCOPE)
                endif()
            endif()
        endif()

        message(STATUS "vcpkg triplet: ${VCPKG_TARGET_TRIPLET}")
    endif()
endfunction()

# Configure Conan integration
function(setup_conan)
    if(CONAN_AVAILABLE)
        # Check if conandata.txt or conanfile.txt exists
        if(EXISTS "${CMAKE_SOURCE_DIR}/conanfile.txt" OR EXISTS "${CMAKE_SOURCE_DIR}/conanfile.py")
            # Run conan install
            execute_process(
                COMMAND ${CONAN_EXECUTABLE} install ${CMAKE_SOURCE_DIR} --build=missing
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                RESULT_VARIABLE CONAN_RESULT
            )

            if(CONAN_RESULT EQUAL 0)
                # Include Conan-generated files
                if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
                    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
                    conan_basic_setup(TARGETS)
                    message(STATUS "Conan dependencies configured")
                endif()
            else()
                message(WARNING "Conan install failed")
            endif()
        endif()
    endif()
endfunction()

# Enhanced dependency fetching with version constraints
function(fetch_dependency_enhanced NAME)
    set(options REQUIRED QUIET SYSTEM_FIRST)
    set(oneValueArgs
        GIT_REPOSITORY GIT_TAG
        FIND_PACKAGE_NAME VERSION MIN_VERSION MAX_VERSION
        VCPKG_NAME CONAN_NAME
    )
    set(multiValueArgs CMAKE_ARGS PATCH_COMMAND)
    cmake_parse_arguments(DEP "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Set default find package name
    if(NOT DEP_FIND_PACKAGE_NAME)
        set(DEP_FIND_PACKAGE_NAME ${NAME})
    endif()

    # Try system package first if requested
    if(DEP_SYSTEM_FIRST OR NOT DEP_GIT_REPOSITORY)
        if(DEP_QUIET)
            find_package(${DEP_FIND_PACKAGE_NAME} ${DEP_VERSION} QUIET)
        else()
            find_package(${DEP_FIND_PACKAGE_NAME} ${DEP_VERSION})
        endif()

        # Check version constraints
        if(${DEP_FIND_PACKAGE_NAME}_FOUND)
            set(version_ok TRUE)

            if(DEP_MIN_VERSION AND DEFINED ${DEP_FIND_PACKAGE_NAME}_VERSION)
                if(${DEP_FIND_PACKAGE_NAME}_VERSION VERSION_LESS ${DEP_MIN_VERSION})
                    set(version_ok FALSE)
                    message(STATUS "System ${DEP_FIND_PACKAGE_NAME} version ${${DEP_FIND_PACKAGE_NAME}_VERSION} is below minimum ${DEP_MIN_VERSION}")
                endif()
            endif()

            if(DEP_MAX_VERSION AND DEFINED ${DEP_FIND_PACKAGE_NAME}_VERSION)
                if(${DEP_FIND_PACKAGE_NAME}_VERSION VERSION_GREATER ${DEP_MAX_VERSION})
                    set(version_ok FALSE)
                    message(STATUS "System ${DEP_FIND_PACKAGE_NAME} version ${${DEP_FIND_PACKAGE_NAME}_VERSION} is above maximum ${DEP_MAX_VERSION}")
                endif()
            endif()

            if(version_ok)
                message(STATUS "Using system ${DEP_FIND_PACKAGE_NAME}: ${${DEP_FIND_PACKAGE_NAME}_VERSION}")
                return()
            else()
                message(STATUS "System ${DEP_FIND_PACKAGE_NAME} version constraints not met, will fetch from source")
            endif()
        endif()
    endif()

    # Try vcpkg if available
    if(VCPKG_AVAILABLE AND DEP_VCPKG_NAME)
        find_package(${DEP_VCPKG_NAME} QUIET)
        if(${DEP_VCPKG_NAME}_FOUND)
            message(STATUS "Using vcpkg ${DEP_VCPKG_NAME}")
            return()
        endif()
    endif()

    # Try Conan if available
    if(CONAN_AVAILABLE AND DEP_CONAN_NAME)
        find_package(${DEP_CONAN_NAME} QUIET)
        if(${DEP_CONAN_NAME}_FOUND)
            message(STATUS "Using Conan ${DEP_CONAN_NAME}")
            return()
        endif()
    endif()

    # Fallback to FetchContent
    if(DEP_GIT_REPOSITORY)
        message(STATUS "Fetching ${NAME} from Git repository")

        include(FetchContent)

        set(fetch_args
            ${NAME}
            GIT_REPOSITORY ${DEP_GIT_REPOSITORY}
        )

        if(DEP_GIT_TAG)
            list(APPEND fetch_args GIT_TAG ${DEP_GIT_TAG})
        endif()

        if(DEP_PATCH_COMMAND)
            list(APPEND fetch_args PATCH_COMMAND ${DEP_PATCH_COMMAND})
        endif()

        FetchContent_Declare(${fetch_args})

        if(DEP_CMAKE_ARGS)
            set(${NAME}_CMAKE_ARGS ${DEP_CMAKE_ARGS})
        endif()

        FetchContent_MakeAvailable(${NAME})

    elseif(DEP_REQUIRED)
        message(FATAL_ERROR "Required dependency ${NAME} not found and no Git repository provided")
    else()
        message(WARNING "Optional dependency ${NAME} not found")
    endif()
endfunction()

# Create a dependency manifest
function(create_dependency_manifest)
    set(MANIFEST_FILE "${CMAKE_SOURCE_DIR}/dependencies.json")

    # Create JSON manifest with dependency information
    set(MANIFEST_CONTENT "{
  \"dependencies\": {
    \"spdlog\": {
      \"version\": \"1.12.0\",
      \"git_repository\": \"https://github.com/gabime/spdlog.git\",
      \"git_tag\": \"v1.12.0\",
      \"vcpkg_name\": \"spdlog\",
      \"conan_name\": \"spdlog\",
      \"system_first\": true
    },
    \"nlohmann_json\": {
      \"version\": \"3.11.3\",
      \"git_repository\": \"https://github.com/nlohmann/json.git\",
      \"git_tag\": \"v3.11.3\",
      \"vcpkg_name\": \"nlohmann-json\",
      \"conan_name\": \"nlohmann_json\",
      \"system_first\": true
    },
    \"googletest\": {
      \"version\": \"1.14.0\",
      \"git_repository\": \"https://github.com/google/googletest.git\",
      \"git_tag\": \"v1.14.0\",
      \"vcpkg_name\": \"gtest\",
      \"conan_name\": \"gtest\",
      \"system_first\": false,
      \"test_only\": true
    }
  }
}")

    file(WRITE ${MANIFEST_FILE} ${MANIFEST_CONTENT})
    message(STATUS "Dependency manifest created: ${MANIFEST_FILE}")
endfunction()

# Load dependencies from manifest
function(load_dependencies_from_manifest)
    set(MANIFEST_FILE "${CMAKE_SOURCE_DIR}/dependencies.json")

    if(EXISTS ${MANIFEST_FILE})
        message(STATUS "Loading dependencies from manifest: ${MANIFEST_FILE}")
        # Note: This is a simplified version. In practice, you'd use a JSON parser
        # For now, we'll manually configure the known dependencies

        # spdlog
        fetch_dependency_enhanced(spdlog
            FIND_PACKAGE_NAME spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.12.0
            VCPKG_NAME spdlog
            CONAN_NAME spdlog
            SYSTEM_FIRST
            QUIET
        )

        # nlohmann_json - try vcpkg first, then system, then local fallback
        find_package(nlohmann_json QUIET)
        if(NOT nlohmann_json_FOUND)
            message(STATUS "nlohmann_json not found via find_package, trying manual search")
            # Try to find it manually (including local include directory)
            find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp
                PATHS ${CMAKE_SOURCE_DIR}/include
                NO_DEFAULT_PATH)
            if(NOT NLOHMANN_JSON_INCLUDE_DIR)
                find_path(NLOHMANN_JSON_INCLUDE_DIR nlohmann/json.hpp)
            endif()

            if(NLOHMANN_JSON_INCLUDE_DIR)
                add_library(nlohmann_json INTERFACE IMPORTED)
                target_include_directories(nlohmann_json INTERFACE ${NLOHMANN_JSON_INCLUDE_DIR})
                add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
                message(STATUS "Found nlohmann_json headers at: ${NLOHMANN_JSON_INCLUDE_DIR}")
            else()
                message(STATUS "nlohmann_json not found, will use cached version if available")
                # Use cached version if available
                if(EXISTS "${CMAKE_SOURCE_DIR}/deps_cache/json_https___github_com_nlohmann_json_git_v3.11.3")
                    message(STATUS "Using cached nlohmann_json")
                    include(FetchContent)
                    FetchContent_Declare(json
                        SOURCE_DIR "${CMAKE_SOURCE_DIR}/deps_cache/json_https___github_com_nlohmann_json_git_v3.11.3"
                    )
                    FetchContent_MakeAvailable(json)
                else()
                    message(STATUS "Fetching nlohmann_json from GitHub")
                    include(FetchContent)
                    FetchContent_Declare(json
                        GIT_REPOSITORY https://github.com/nlohmann/json.git
                        GIT_TAG v3.11.3
                        SOURCE_DIR "${CMAKE_SOURCE_DIR}/deps_cache/json_https___github_com_nlohmann_json_git_v3.11.3"
                    )
                    FetchContent_MakeAvailable(json)
                endif()
            endif()
        else()
            message(STATUS "Using system nlohmann_json")
        endif()

        # googletest (for testing)
        if(BUILD_TESTING)
            find_package(GTest QUIET)
            if(NOT GTest_FOUND)
                message(STATUS "GTest not found, will use cached version if available")
                if(EXISTS "${CMAKE_SOURCE_DIR}/deps_cache/googletest_https___github_com_google_googletest_git_v1.14.0")
                    message(STATUS "Using cached googletest")
                    include(FetchContent)
                    FetchContent_Declare(googletest
                        SOURCE_DIR "${CMAKE_SOURCE_DIR}/deps_cache/googletest_https___github_com_google_googletest_git_v1.14.0"
                    )
                    FetchContent_MakeAvailable(googletest)
                else()
                    message(STATUS "Cached googletest not found, testing may not work")
                endif()
            else()
                message(STATUS "Using system GTest")
            endif()
        endif()

    else()
        message(STATUS "No dependency manifest found, using default configuration")
        # Fallback to original dependency setup
        setup_core_dependencies()
        setup_test_dependencies()
    endif()
endfunction()

# Setup enhanced package management
function(setup_enhanced_package_management)
    detect_package_managers()
    setup_vcpkg()
    setup_conan()

    # Create manifest if it doesn't exist
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/dependencies.json")
        create_dependency_manifest()
    endif()

    # Load dependencies
    load_dependencies_from_manifest()

    message(STATUS "Enhanced package management configured")
endfunction()
