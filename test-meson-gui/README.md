# test-meson-gui

一个使用Qt开发的C++ GUI应用，由CPP-Scaffold创建�?

## 功能特点

- 现代化的C++17 GUI应用
- 使用Qt作为界面框架
- 集成spdlog日志系统
- meson构建系统
- vcpkg包管理
- 集成doctest测试框架


## 构建说明

### 依赖�?

- C++17兼容编译�?
- Qt 6.x
- spdlog
- meson 构建系统
- vcpkg


### 编译步骤

```bash
# 配置项目
meson setup build

# 编译
cd build
meson compile
```

### 运行测试

```bash
cd build
meson test
```


### 运行应用

```bash
cd build
./test-meson-gui
```

## 项目结构

```
test-meson-gui/
├── include/               # 头文件目�?
�?  └── test-meson-gui/
�?      ├── application.h  # 应用�?
�?      ├── main_window.h  # 主窗�?
�?      └── logging.h      # 日志工具
├── src/                   # 源文件目�?
�?  ├── main.cpp           # 程序入口�?
�?  ├── application.cpp    # 应用实现
�?  ├── main_window.cpp    # 主窗口实�?
�?  └── logging.cpp        # 日志工具实现
├── ui/                    # UI设计文件
�?  └── main_window.ui  # 主窗口界面设�?
├── resources/             # 资源文件目录
├── tests/                 # 测试目录
�?  └── test_main.cpp       # 测试入口
├── README.md              # 项目说明文档
└── CMakeLists.txt          # CMake构建脚本
```

## 使用spdlog日志系统

项目集成了spdlog日志系统，提供了方便的日志宏供项目使用：

```cpp
// 示例用法
SPDLOG_TRACE("这是一个跟踪日�?);
SPDLOG_DEBUG("这是一个调试日�?);
SPDLOG_INFO("这是一个信息日�?);
SPDLOG_WARN("这是一个警告日�?);
SPDLOG_ERROR("这是一个错误日�? {0}", error_code);
SPDLOG_CRITICAL("这是一个严重错误日�?);
```

## 许可�?

此项目使用MIT许可�?- 详见LICENSE文件