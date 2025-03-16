# 检测<filesystem>是否可用并设置Filesystem_FOUND

# 保存原始变量，以便之后恢复
set(_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
set(_CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS})
set(_CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})

# 尝试编译一个简单的filesystem示例
include(CheckCXXSourceCompiles)

# 首先检查是否无需链接任何库即可使用
set(CMAKE_REQUIRED_FLAGS "-std=c++17")
check_cxx_source_compiles(
  "
  #include <filesystem>
  int main() {
    auto path = std::filesystem::current_path();
    return 0;
  }
  "
  Filesystem_HEADER_ONLY
)

if(Filesystem_HEADER_ONLY)
  # 无需链接任何库
  set(Filesystem_FOUND TRUE)
else()
  # 尝试链接stdc++fs
  set(CMAKE_REQUIRED_LIBRARIES stdc++fs)
  check_cxx_source_compiles(
    "
    #include <filesystem>
    int main() {
      auto path = std::filesystem::current_path();
      return 0;
    }
    "
    Filesystem_WITH_STDCXXFS
  )
  
  if(Filesystem_WITH_STDCXXFS)
    # 链接stdc++fs
    set(Filesystem_FOUND TRUE)
    set(Filesystem_LIBRARIES stdc++fs)
  else()
    # 尝试链接c++fs
    set(CMAKE_REQUIRED_LIBRARIES c++fs)
    check_cxx_source_compiles(
      "
      #include <filesystem>
      int main() {
        auto path = std::filesystem::current_path();
        return 0;
      }
      "
      Filesystem_WITH_CXXFS
    )
    
    if(Filesystem_WITH_CXXFS)
      # 链接c++fs
      set(Filesystem_FOUND TRUE)
      set(Filesystem_LIBRARIES c++fs)
    else()
      set(Filesystem_FOUND FALSE)
    endif()
  endif()
endif()

# 恢复原始变量
set(CMAKE_REQUIRED_FLAGS ${_CMAKE_REQUIRED_FLAGS})
set(CMAKE_REQUIRED_DEFINITIONS ${_CMAKE_REQUIRED_DEFINITIONS})
set(CMAKE_REQUIRED_LIBRARIES ${_CMAKE_REQUIRED_LIBRARIES})

if(Filesystem_FOUND)
  if(NOT TARGET std::filesystem)
    add_library(std::filesystem INTERFACE IMPORTED)
    if(DEFINED Filesystem_LIBRARIES)
      target_link_libraries(std::filesystem INTERFACE ${Filesystem_LIBRARIES})
    endif()
  endif()
endif()

# 处理标准FIND_PACKAGE参数
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Filesystem
  REQUIRED_VARS Filesystem_FOUND
)