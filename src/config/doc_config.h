#pragma once

#include "../cli/cli_parser.h"

#include <string>

class DocConfig {
public:
  // 为项目创建文档配置
  static bool createDocConfig(const std::string &projectPath,
                              const CliOptions &options);

private:
  // 创建Doxygen配置
  static bool createDoxygenConfig(const std::string &projectPath,
                                  const CliOptions &options);

  // 创建Sphinx配置（如果需要）
  static bool createSphinxConfig(const std::string &projectPath,
                                 const CliOptions &options);

  // 获取Doxyfile内容
  static std::string getDoxyfileContent(const CliOptions &options);

  // 获取文档生成脚本内容
  static std::string getDocGenScriptContent(const CliOptions &options);
};