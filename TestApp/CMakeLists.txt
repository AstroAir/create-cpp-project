cmake_minimum_required(VERSION 3.15)
project(TestApp VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compile commands for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Package manager integration

  # vcpkg integration
  if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
  endif()
  
# Dependencies
find_package(fmt REQUIRED)

# Source files
set(SOURCES
  src/main.cpp
  # Add more source files here
)

# Include directories
include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

# Library target (for reuse in tests)
add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})
target_include_directories(${PROJECT_NAME}_lib
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(${PROJECT_NAME}_lib PUBLIC fmt::fmt)

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

# Add compiler warnings
include(CheckCXXCompilerFlag)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
endif()

  # Installation
  include(GNUInstallDirs)
  install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

  # Generate export header if building shared libraries
  include(GenerateExportHeader)
  option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
  if(BUILD_SHARED_LIBS)
    generate_export_header(${PROJECT_NAME}_lib
      EXPORT_MACRO_NAME ${PROJECT_NAME}_API
      EXPORT_FILE_NAME include/${PROJECT_NAME}/export.h
    )
  endif()

  # CPack configuration for package generation
  option(BUILD_PACKAGES "Configure for package generation with CPack" OFF)
  if(BUILD_PACKAGES)
    set(CPACK_PACKAGE_VENDOR "Your Organization")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} - A C++ application")
    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
    include(CPack)
  endif()
  