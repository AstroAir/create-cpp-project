#pragma once
#include "../cli/cli_parser.h"
#include <string>
#include <vector>

class CIConfig {
public:
  // 为指定的CI/CD系统创建配置文件
  static bool createCIConfig(const std::string &projectPath,
                             const std::string &ciType,
                             const CliOptions &options);

  // 创建所有指定的CI/CD配置
  static bool createCIConfigs(const std::string &projectPath,
                              const std::vector<CiSystem> &ciTypes,
                              const CliOptions &options);

private:
  // 创建GitHub Actions配置
  static bool createGitHubActionsConfig(const std::string &projectPath,
                                        const CliOptions &options);

  // 创建GitLab CI配置
  static bool createGitLabCIConfig(const std::string &projectPath,
                                   const CliOptions &options);

  // 创建Travis CI配置
  static bool createTravisCIConfig(const std::string &projectPath,
                                   const CliOptions &options);

  // 创建AppVeyor配置
  static bool createAppVeyorConfig(const std::string &projectPath,
                                   const CliOptions &options);

  // 获取GitHub Actions的workflow YAML内容
  static std::string getGitHubActionsYamlContent(const CliOptions &options);

  // 获取GitLab CI的.gitlab-ci.yml内容
  static std::string getGitLabCIYamlContent(const CliOptions &options);

  // 获取Travis CI的.travis.yml内容
  static std::string getTravisCIYamlContent(const CliOptions &options);

  // 获取AppVeyor的appveyor.yml内容
  static std::string getAppVeyorYamlContent(const CliOptions &options);
};
