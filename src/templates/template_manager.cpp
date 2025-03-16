#include "template_manager.h"

#include "console_template.h"
#include "gui_template.h"
#include "library_template.h"

#include <iostream>

TemplateManager::TemplateManager() {
  // 注册模板工厂
  templateFactories_["console"] = [](const CliOptions &options) {
    return std::make_unique<ConsoleTemplate>(options);
  };

  templateFactories_["lib"] = [](const CliOptions &options) {
    return std::make_unique<LibraryTemplate>(options);
  };

  templateFactories_["gui"] = [](const CliOptions &options) {
    return std::make_unique<GuiTemplate>(options);
  };
}

bool TemplateManager::createProject(const CliOptions &options) {
  // 查找对应的模板工厂
  auto it = templateFactories_.find(options.templateType);
  if (it == templateFactories_.end()) {
    std::cerr << "错误: 未知的项目模板类型 '" << options.templateType
              << "'。\n";
    return false;
  }

  // 创建模板
  auto template_ptr = it->second(options);

  // 创建项目
  return template_ptr->create();
}