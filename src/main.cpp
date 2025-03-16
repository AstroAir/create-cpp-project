#include "cli/cli_parser.h"
#include "templates/template_manager.h"

#include <iostream>


int main(int argc, char *argv[]) {
  try {
    // 解析命令行参数
    CliOptions options = CliParser::parse(argc, argv);

    // 显示帮助
    if (options.showHelp) {
      CliParser::showHelp();
      return 0;
    }

    // 显示版本
    if (options.version) {
      CliParser::showVersion();
      return 0;
    }

    // 创建项目
    TemplateManager templateManager;
    if (!templateManager.createProject(options)) {
      return 1;
    }

  } catch (const std::exception &e) {
    std::cerr << "错误: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}