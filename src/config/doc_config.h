#pragma once

#include <string>

#include "../cli/cli_parser.h"


class DocConfig {
   public:
    // 为项目创建文档配置
    static bool createDocConfig(const std::string& projectPath, const CliOptions& options);

   private:
    // 创建Doxygen配置
    static bool createDoxygenConfig(const std::string& projectPath, const CliOptions& options);

    // 创建Sphinx配置（如果需要）
    static bool createSphinxConfig(const std::string& projectPath, const CliOptions& options);

    // 获取Doxyfile内容
    static std::string getDoxyfileContent(const CliOptions& options);

    // 获取文档生成脚本内容
    static std::string getDocGenScriptContent(const CliOptions& options);

    // Sphinx configuration content generators
    static std::string getSphinxConfContent(const CliOptions& options);
    static std::string getSphinxIndexContent(const CliOptions& options);
    static std::string getSphinxRequirementsContent();
    static std::string getSphinxMakefileContent();
    static std::string getSphinxMakeBatContent();
};
