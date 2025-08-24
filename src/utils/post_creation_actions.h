#pragma once

#include "../cli/cli_parser.h"
#include <string>
#include <vector>
#include <functional>

namespace utils {

enum class ActionResult {
  Success,
  Warning,
  Error,
  Skipped
};

struct ActionStatus {
  std::string name;
  std::string description;
  ActionResult result;
  std::string message;
  double duration = 0.0; // in seconds
};

class PostCreationActions {
public:
  static PostCreationActions& getInstance();
  
  // Main execution method
  bool executeAll(const CliOptions& options);
  
  // Individual actions
  ActionStatus installDependencies(const CliOptions& options);
  ActionStatus validateProjectStructure(const CliOptions& options);
  ActionStatus runInitialBuild(const CliOptions& options);
  ActionStatus setupDevelopmentEnvironment(const CliOptions& options);
  ActionStatus generateDocumentation(const CliOptions& options);
  ActionStatus runInitialTests(const CliOptions& options);
  ActionStatus setupContinuousIntegration(const CliOptions& options);
  ActionStatus createInitialCommit(const CliOptions& options);
  
  // Configuration
  void setVerbose(bool verbose) { verbose_ = verbose; }
  void setSkipOptional(bool skip) { skipOptional_ = skip; }
  void setParallel(bool parallel) { parallel_ = parallel; }
  
  // Progress reporting
  void setProgressCallback(std::function<void(const std::string&, int)> callback);
  
  // Action management
  void enableAction(const std::string& actionName, bool enabled = true);
  void disableAction(const std::string& actionName);
  bool isActionEnabled(const std::string& actionName) const;
  
  // Results
  std::vector<ActionStatus> getResults() const { return results_; }
  bool hasErrors() const;
  bool hasWarnings() const;
  void printSummary() const;
  
private:
  PostCreationActions() = default;
  
  // Helper methods
  ActionStatus executeAction(const std::string& name, 
                           const std::string& description,
                           std::function<bool()> action);
  
  bool runCommand(const std::string& command, const std::string& workingDir = "");
  bool checkCommandExists(const std::string& command);
  bool validateFile(const std::string& path, const std::string& expectedContent = "");
  bool validateDirectory(const std::string& path);
  
  // Build system specific methods
  bool buildWithCMake(const std::string& projectPath);
  bool buildWithMeson(const std::string& projectPath);
  bool buildWithBazel(const std::string& projectPath);
  bool buildWithMake(const std::string& projectPath);
  
  // Package manager specific methods
  bool installWithVcpkg(const std::string& projectPath);
  bool installWithConan(const std::string& projectPath);
  bool installWithSpack(const std::string& projectPath);
  
  // Testing methods
  bool runCTests(const std::string& projectPath);
  bool runCustomTests(const std::string& projectPath);
  
  // Documentation methods
  bool generateDoxygen(const std::string& projectPath);
  bool generateSphinx(const std::string& projectPath);
  
  // Development environment setup
  bool setupVSCode(const std::string& projectPath);
  bool setupCLion(const std::string& projectPath);
  bool setupVS(const std::string& projectPath);
  
  // CI/CD setup
  bool setupGitHubActions(const std::string& projectPath);
  bool setupGitLabCI(const std::string& projectPath);
  bool setupTravisCI(const std::string& projectPath);
  
  // Member variables
  bool verbose_ = false;
  bool skipOptional_ = false;
  bool parallel_ = false;
  std::vector<ActionStatus> results_;
  std::unordered_map<std::string, bool> enabledActions_;
  std::function<void(const std::string&, int)> progressCallback_;
};
