#pragma once
#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace utils {

// System information structure
struct SystemInfo {
    std::string operatingSystem;
    std::string architecture;
    std::string kernelVersion;
    std::string hostname;
    std::string username;
    size_t totalMemoryMB;
    size_t availableMemoryMB;
    size_t cpuCores;
    std::string cpuModel;
    std::vector<std::string> environmentVariables;
};

// Compiler information
struct CompilerInfo {
    std::string name;
    std::string version;
    std::string path;
    std::vector<std::string> supportedStandards;
    bool available;
};

// Tool information
struct ToolInfo {
    std::string name;
    std::string version;
    std::string path;
    bool available;
    std::string errorMessage;
};

// System requirements
struct SystemRequirements {
    size_t minMemoryMB = 1024;
    size_t minDiskSpaceMB = 1024;
    std::vector<std::string> requiredTools;
    std::vector<std::string> recommendedTools;
    std::string minOperatingSystem;
};

class SystemUtils {
   public:
    // System information
    static SystemInfo getSystemInfo();
    static std::string getOperatingSystem();
    static std::string getArchitecture();
    static std::string getKernelVersion();
    static std::string getHostname();
    static std::string getUsername();
    static size_t getTotalMemory();
    static size_t getAvailableMemory();
    static size_t getCpuCoreCount();
    static std::string getCpuModel();

    // Environment variables
    static std::optional<std::string> getEnvironmentVariable(const std::string& name);
    static bool setEnvironmentVariable(const std::string& name, const std::string& value);
    static std::unordered_map<std::string, std::string> getAllEnvironmentVariables();
    static std::vector<std::filesystem::path> getPathDirectories();

    // Compiler detection and validation
    static std::vector<CompilerInfo> detectAvailableCompilers();
    static CompilerInfo detectCompiler(const std::string& compilerName);
    static bool isCompilerAvailable(const std::string& compilerName);
    static std::string getCompilerVersion(const std::string& compilerPath);
    static std::vector<std::string> getSupportedCppStandards(const std::string& compilerPath);

    // Tool detection and validation
    static ToolInfo getToolInfo(const std::string& toolName);
    static bool isToolAvailable(const std::string& toolName);
    static std::string getToolVersion(const std::string& toolName);
    static std::filesystem::path findToolInPath(const std::string& toolName);
    static std::vector<ToolInfo> checkRequiredTools(const std::vector<std::string>& tools);

    // Build system detection
    static bool isCMakeAvailable();
    static bool isNinjaAvailable();
    static bool isMakeAvailable();
    static bool isXMakeAvailable();
    static bool isMesonAvailable();
    static bool isBazelAvailable();

    // Package manager detection
    static bool isVcpkgAvailable();
    static bool isConanAvailable();
    static bool isPackageManagerAvailable(const std::string& packageManager);
    static std::filesystem::path getVcpkgRoot();
    static std::filesystem::path getConanHome();

    // Directory utilities
    static std::filesystem::path getTempDirectory();
    static std::filesystem::path getCurrentDirectory();
    static std::filesystem::path getHomeDirectory();

    // System validation
    static bool validateSystemRequirements(const SystemRequirements& requirements);
    static std::vector<std::string> checkMissingDependencies(
            const std::vector<std::string>& dependencies);
    static bool hasMinimumDiskSpace(const std::filesystem::path& path, size_t requiredMB);
    static bool hasMinimumMemory(size_t requiredMB);

    // Process and execution utilities
    static std::optional<std::string> executeCommand(const std::string& command);
    static std::optional<std::string> executeCommandWithOutput(
            const std::string& command, const std::vector<std::string>& args = {});
    static bool isProcessRunning(const std::string& processName);
    static std::vector<std::string> getRunningProcesses();
    static int getCurrentProcessId();
    static std::string getCurrentProcessName();

    // File system information
    static std::vector<std::filesystem::path> getMountPoints();
    static std::uintmax_t getDiskSpace(const std::filesystem::path& path);
    static std::uintmax_t getFreeDiskSpace(const std::filesystem::path& path);
    static std::filesystem::space_info getSpaceInfo(const std::filesystem::path& path);

    // Network utilities
    static bool isInternetConnected();
    static bool canReachHost(const std::string& hostname, int port = 80);
    static std::string getLocalIPAddress();
    static std::vector<std::string> getNetworkInterfaces();

    // Performance monitoring
    static double getCpuUsage();
    static size_t getMemoryUsage();
    static std::chrono::milliseconds getUptime();
    static double getSystemLoad();

    // Development environment detection
    static std::vector<std::string> detectInstalledIDEs();
    static bool isGitAvailable();
    static std::string getGitVersion();
    static bool isPythonAvailable();
    static std::string getPythonVersion();
    static bool isNodeJSAvailable();
    static std::string getNodeJSVersion();

    // Shell and terminal utilities
    static std::string getDefaultShell();
    static bool isRunningInTerminal();
    static std::pair<int, int> getTerminalSize();
    static bool supportsAnsiColors();
    static bool supportsUnicode();

    // Security and permissions
    static bool hasAdministratorPrivileges();
    static bool canWriteToSystemDirectories();
    static std::vector<std::string> getUserGroups();
    static bool isFileExecutable(const std::filesystem::path& filePath);

    // Locale and internationalization
    static std::string getCurrentLocale();
    static std::string getSystemLanguage();
    static std::string getSystemTimezone();
    static std::vector<std::string> getAvailableLocales();

   private:
    static std::string executeSystemCommand(const std::string& command);
    static bool commandExists(const std::string& command);
    static std::string extractVersionFromOutput(const std::string& output);
};

}  // namespace utils
