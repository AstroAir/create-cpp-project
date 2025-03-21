#pragma once
#include "../cli/cli_parser.h"
#include "template_base.h"
#include <functional>
#include <map>
#include <memory>
#include <string>

class TemplateManager {
public:
  TemplateManager();

  // 创建项目
  bool createProject(const CliOptions &options);

private:
  // 模板工厂函数类型
  using TemplateFactory =
      std::function<std::unique_ptr<TemplateBase>(const CliOptions &)>;

  // 模板工厂映射
  std::map<std::string, TemplateFactory> templateFactories_;
};