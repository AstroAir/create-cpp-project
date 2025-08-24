#include "template_manager.h"

#include "console_template.h"
#include "gui_template.h"
#include "library_template.h"
#include "header_only_lib_template.h"
// #include "multi_executable_template.h"
#include "network_template.h"
#include "custom_template.h"
#include "webservice_template.h"
#include "embedded_template.h"
#include "gameengine_template.h"
#include "../utils/framework_integration.h"

#include <iostream>
#include <spdlog/spdlog.h>

TemplateManager::TemplateManager() {
  // 注册模板工厂
  templateFactories_["console"] = [](const CliOptions &options) {
    return std::make_unique<ConsoleTemplate>(options);
  };

  templateFactories_["lib"] = [](const CliOptions &options) {
    return std::make_unique<LibraryTemplate>(options);
  };

  templateFactories_["header-only-lib"] = [](const CliOptions &options) {
    return std::make_unique<HeaderOnlyLibTemplate>(options);
  };

  // templateFactories_["multi-executable"] = [](const CliOptions &options) {
  //   return std::make_unique<MultiExecutableTemplate>(options);
  // };

  templateFactories_["gui"] = [](const CliOptions &options) {
    return std::make_unique<GuiTemplate>(options);
  };

  templateFactories_["network"] = [](const CliOptions &options) {
    return std::make_unique<NetworkTemplate>(options);
  };

  // Framework-specific templates
  templateFactories_["qt-app"] = [](const CliOptions &options) {
    return createFrameworkTemplate(options, utils::FrameworkIntegration::Framework::Qt);
  };

  templateFactories_["sfml-app"] = [](const CliOptions &options) {
    return createFrameworkTemplate(options, utils::FrameworkIntegration::Framework::SFML);
  };

  templateFactories_["boost-app"] = [](const CliOptions &options) {
    return createFrameworkTemplate(options, utils::FrameworkIntegration::Framework::Boost);
  };

  templateFactories_["test-project"] = [](const CliOptions &options) {
    return createFrameworkTemplate(options, utils::FrameworkIntegration::Framework::GoogleTest);
  };

  templateFactories_["webservice"] = [](const CliOptions &options) {
    return std::make_unique<WebServiceTemplate>(options);
  };

  templateFactories_["embedded"] = [](const CliOptions &options) {
    return std::make_unique<EmbeddedTemplate>(options);
  };

  templateFactories_["gameengine"] = [](const CliOptions &options) {
    return std::make_unique<GameEngineTemplate>(options);
  };
}

bool TemplateManager::createProject(const CliOptions &options) {
  try {
    // 将枚举转换为字符串
    std::string templateTypeStr = std::string(enums::to_string(options.templateType));

    // 查找对应的模板工厂
    auto it = templateFactories_.find(templateTypeStr);
    if (it == templateFactories_.end()) {
      std::cerr << "错误: 未知的项目模板类型 '" << templateTypeStr
                << "'。\n";
      return false;
    }

    // 创建模板
    auto template_ptr = it->second(options);

    // 创建项目
    return template_ptr->create();
  } catch (const std::exception& e) {
    spdlog::error("Exception during project creation: {}", e.what());
    std::cerr << "错误: 创建项目时发生异常: " << e.what() << "\n";
    return false;
  }
}

std::unique_ptr<TemplateBase> TemplateManager::createFrameworkTemplate(const CliOptions& options, utils::FrameworkIntegration::Framework framework) {
  // Create a custom template that integrates the specified framework
  auto customTemplate = std::make_unique<CustomTemplate>(options);

  // Set up the framework integration
  std::filesystem::path projectPath(options.projectName);

  // Install and configure the framework
  utils::FrameworkIntegration::installFramework(framework, projectPath, utils::FrameworkIntegration::PackageManager::vcpkg);
  utils::FrameworkIntegration::configureFramework(framework, projectPath);

  // Add framework-specific template variables
  auto frameworkInfo = utils::FrameworkIntegration::getFrameworkInfo(framework);
  if (frameworkInfo) {
    customTemplate->setTemplateVariable("FRAMEWORK_NAME", frameworkInfo->name);
    customTemplate->setTemplateVariable("FRAMEWORK_VERSION", frameworkInfo->version);
  }

  return customTemplate;
}
