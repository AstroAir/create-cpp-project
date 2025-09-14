#pragma once
#include <string>

#include "../cli/cli_parser.h"


class CodeStyleConfig {
   public:
    // 为项目创建代码风格配置
    static bool createCodeStyleConfig(const std::string& projectPath, const CliOptions& options);

   private:
    // 创建clang-format配置
    static bool createClangFormatConfig(const std::string& projectPath);

    // 创建clang-tidy配置
    static bool createClangTidyConfig(const std::string& projectPath);

    // 创建EditorConfig配置
    static bool createEditorConfig(const std::string& projectPath);

    // 获取clang-format配置内容
    static std::string getClangFormatContent();

    // 获取clang-tidy配置内容
    static std::string getClangTidyContent();

    // 获取EditorConfig配置内容
    static std::string getEditorConfigContent();
};
