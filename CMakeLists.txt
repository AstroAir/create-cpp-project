cmake_minimum_required(VERSION 3.14)
project(cpp-scaffold VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 在Debug模式下启用更多警告
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(MSVC)
        add_compile_options(/W4 /WX)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

# 如果是Windows，确保UNICODE有定义
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# 第三方库设置
include(FetchContent)

# 设置全局缓存目录
set(DEPS_CACHE_DIR "${CMAKE_SOURCE_DIR}/deps_cache" CACHE PATH "依赖项缓存目录")
file(MAKE_DIRECTORY ${DEPS_CACHE_DIR})

# 自定义函数用于缓存下载的依赖项
function(fetch_and_cache NAME GIT_REPO GIT_TAG)
    # 创建缓存目录路径，替换所有特殊字符
    string(REPLACE "/" "_" SAFE_REPO_NAME ${GIT_REPO})
    string(REPLACE ":" "_" SAFE_REPO_NAME ${SAFE_REPO_NAME})
    string(REPLACE "." "_" SAFE_REPO_NAME ${SAFE_REPO_NAME})
    set(CACHE_DIR "${DEPS_CACHE_DIR}/${NAME}_${SAFE_REPO_NAME}_${GIT_TAG}")

    # 检查缓存目录是否存在
    if(EXISTS ${CACHE_DIR})
        message(STATUS "使用缓存的依赖项: ${NAME} (${GIT_TAG})")
        FetchContent_Declare(
            ${NAME}
            SOURCE_DIR ${CACHE_DIR}
        )
    else()
        message(STATUS "下载依赖项: ${NAME} (${GIT_TAG})")
        FetchContent_Declare(
            ${NAME}
            GIT_REPOSITORY ${GIT_REPO}
            GIT_TAG ${GIT_TAG}
            SOURCE_DIR ${CACHE_DIR}
        )
    endif()

    # 使依赖项可用
    FetchContent_MakeAvailable(${NAME})
endfunction()

# 检查系统是否已安装spdlog
find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
    message(STATUS "System spdlog not found, will use fetched version")
    # 使用缓存功能添加spdlog
    fetch_and_cache(
        spdlog
        https://github.com/gabime/spdlog.git
        v1.12.0
    )
else()
    message(STATUS "Using system spdlog: ${spdlog_VERSION}")
endif()

# 检查系统是否已安装nlohmann_json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "System nlohmann_json not found, will use fetched version")
    # 使用缓存功能添加nlohmann/json
    fetch_and_cache(
        json
        https://github.com/nlohmann/json.git
        v3.11.3
    )
else()
    message(STATUS "Using system nlohmann_json: ${nlohmann_json_VERSION}")
endif()

# 源文件目录
set(CORE_SOURCES
    src/main.cpp
    src/cli/cli_parser.cpp
    src/cli/project_wizard.cpp
    src/templates/template_manager.cpp
    src/templates/template_base.cpp
    src/templates/console_template.cpp
    src/templates/library_template.cpp
    src/templates/header_only_lib_template.cpp
    # src/templates/multi_executable_template.cpp
    src/templates/gui_template.cpp
    src/templates/network_template.cpp
    src/templates/custom_template.cpp
    src/templates/webservice_template.cpp
    src/templates/embedded_template.cpp
    src/utils/file_utils.cpp
    src/utils/string_utils.cpp
    src/utils/terminal_utils.cpp
    src/utils/git_utils.cpp
    src/utils/framework_integration.cpp
    src/utils/progress_indicator.cpp
    src/utils/interactive_menu.cpp
    # src/utils/dependency_manager.cpp
    # src/utils/post_creation_actions.cpp
)

# 配置工具源文件
set(CONFIG_SOURCES
    src/config/editor_config.cpp
    src/config/ci_config.cpp
    src/config/code_style_config.cpp
    src/config/doc_config.cpp
    src/config/config_manager.cpp
    src/config/user_preferences.cpp
    src/config/interactive_config.cpp
    src/config/project_profiles.cpp
    src/config/config_validator.cpp
)

set(TESTING_SOURCES
    src/testing/test_framework_manager.cpp
)

set(DOCUMENTATION_SOURCES
    src/documentation/doc_generator.cpp
)

# 所有源文件
set(SOURCES ${CORE_SOURCES} ${CONFIG_SOURCES} ${TESTING_SOURCES} ${DOCUMENTATION_SOURCES})

# 包含目录
include_directories(src)

# 检测文件系统库要求
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        find_package(Filesystem REQUIRED)
        if(NOT Filesystem_FOUND)
            message(FATAL_ERROR "Filesystem library not found. Use a newer compiler or provide stdc++fs library.")
        endif()
        set(FILESYSTEM_LIB stdc++fs)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        set(FILESYSTEM_LIB c++fs)
    endif()
endif()

# 生成可执行文件
add_executable(cpp-scaffold ${SOURCES})

# 链接所需库
if(DEFINED FILESYSTEM_LIB)
    target_link_libraries(cpp-scaffold PRIVATE ${FILESYSTEM_LIB})
endif()

# 链接spdlog和nlohmann_json
if(spdlog_FOUND)
    target_link_libraries(cpp-scaffold PRIVATE spdlog::spdlog)
else()
    target_link_libraries(cpp-scaffold PRIVATE spdlog::spdlog)
endif()

if(nlohmann_json_FOUND)
    target_link_libraries(cpp-scaffold PRIVATE nlohmann_json::nlohmann_json)
else()
    target_link_libraries(cpp-scaffold PRIVATE nlohmann_json::nlohmann_json)
endif()

# Windows平台需要额外的库
if(WIN32)
    target_link_libraries(cpp-scaffold PRIVATE shlwapi)
endif()

# 处理安装
install(TARGETS cpp-scaffold RUNTIME DESTINATION bin)

# 安装文档和配置文件
install(FILES README.md LICENSE DESTINATION share/cpp-scaffold)

# 制作可分发包
set(CPACK_PACKAGE_NAME "cpp-scaffold")
set(CPACK_PACKAGE_VENDOR "CPP-Scaffold Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ Project Scaffolding Tool")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CPP-Scaffold")

# 设置平台特定打包选项
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_MODIFY_PATH ON)
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "CPP-Scaffold Team")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
    set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
endif()

include(CPack)

# 添加单元测试 (如果BUILD_TESTING被启用)
option(BUILD_TESTING "Build the testing tree." ON)
if(BUILD_TESTING)
    enable_testing()

    # 添加Google Test
    fetch_and_cache(
        googletest
        https://github.com/google/googletest.git
        v1.14.0
    )

    add_subdirectory(tests)
endif()
