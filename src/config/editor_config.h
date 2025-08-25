#pragma once

#include "../cli/cli_parser.h"

#include <map>
#include <string>
#include <vector>

class EditorConfigManager {
public:
  // 为指定的编辑器创建配置文件
  static bool createEditorConfig(const std::string &projectPath,
                                 const std::string &editorType,
                                 const CliOptions &options);

  // 创建所有指定的编辑器配置
  static bool createEditorConfigs(const std::string &projectPath,
                                  const std::vector<EditorConfig> &editorTypes,
                                  const CliOptions &options);

private:
  // 创建VS Code配置
  static bool createVSCodeConfig(const std::string &projectPath,
                                 const CliOptions &options);

  // 创建CLion配置
  static bool createCLionConfig(const std::string &projectPath,
                                const CliOptions &options);

  // 创建Visual Studio配置
  static bool createVisualStudioConfig(const std::string &projectPath,
                                       const CliOptions &options);

  // 获取VS Code的launch.json内容
  static std::string getVSCodeLaunchJsonContent(const CliOptions &options);

  // 获取VS Code的tasks.json内容
  static std::string getVSCodeTasksJsonContent(const CliOptions &options);

  // 获取VS Code的settings.json内容
  static std::string getVSCodeSettingsJsonContent(const CliOptions &options);

  // 获取VS Code的c_cpp_properties.json内容
  static std::string
  getVSCodeCppPropertiesJsonContent(const CliOptions &options);

  // 获取CLion的CMakePresets.json内容 (for CMake projects)
  static std::string getCLionCMakePresetsJsonContent(const CliOptions &options);

  // 获取Visual Studio的.vsconfig内容
  static std::string getVisualStudioConfigContent(const CliOptions &options);

  // 获取Visual Studio的Directory.Build.props内容
  static std::string getVisualStudioPropsContent(const CliOptions &options);
};
