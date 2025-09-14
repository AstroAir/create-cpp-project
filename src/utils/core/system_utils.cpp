#include "../core/system_utils.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <iphlpapi.h>
#include <psapi.h>
#include <windows.h>
#include <winsock2.h>
#ifdef _MSC_VER
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "psapi.lib")
#endif
#else
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <pwd.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>
#endif

namespace utils {

SystemInfo SystemUtils::getSystemInfo() {
    SystemInfo info;
    info.operatingSystem = getOperatingSystem();
    info.architecture = getArchitecture();
    info.kernelVersion = getKernelVersion();
    info.hostname = getHostname();
    info.username = getUsername();
    info.totalMemoryMB = getTotalMemory();
    info.availableMemoryMB = getAvailableMemory();
    info.cpuCores = getCpuCoreCount();
    info.cpuModel = getCpuModel();
    return info;
}

std::string SystemUtils::getOperatingSystem() {
#ifdef _WIN32
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (GetVersionEx((OSVERSIONINFO*)&osvi)) {
        std::stringstream ss;
        ss << "Windows " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion;
        return ss.str();
    }
    return "Windows (Unknown Version)";
#else
    struct utsname unameData;
    if (uname(&unameData) == 0) {
        return std::string(unameData.sysname) + " " + std::string(unameData.release);
    }
    return "Unix-like (Unknown)";
#endif
}

std::string SystemUtils::getArchitecture() {
#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    switch (sysinfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            return "x86_64";
        case PROCESSOR_ARCHITECTURE_INTEL:
            return "x86";
        case PROCESSOR_ARCHITECTURE_ARM:
            return "ARM";
        case PROCESSOR_ARCHITECTURE_ARM64:
            return "ARM64";
        default:
            return "Unknown";
    }
#else
    struct utsname unameData;
    if (uname(&unameData) == 0) {
        return std::string(unameData.machine);
    }
    return "Unknown";
#endif
}

std::string SystemUtils::getKernelVersion() {
#ifdef _WIN32
    return getOperatingSystem();  // Windows doesn't distinguish kernel version easily
#else
    struct utsname unameData;
    if (uname(&unameData) == 0) {
        return std::string(unameData.version);
    }
    return "Unknown";
#endif
}

std::string SystemUtils::getHostname() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return std::string(hostname);
    }
    return "Unknown";
}

std::string SystemUtils::getUsername() {
#ifdef _WIN32
    wchar_t username[UNLEN + 1];
    DWORD size = UNLEN + 1;
    if (GetUserNameW(username, &size)) {
        // Convert wide string to narrow string
        int len = WideCharToMultiByte(CP_UTF8, 0, username, -1, nullptr, 0, nullptr, nullptr);
        if (len > 0) {
            std::string result(len - 1, '\0');
            WideCharToMultiByte(CP_UTF8, 0, username, -1, &result[0], len, nullptr, nullptr);
            return result;
        }
    }
    return "Unknown";
#else
    char* username = getenv("USER");
    if (username) {
        return std::string(username);
    }

    struct passwd* pw = getpwuid(getuid());
    if (pw) {
        return std::string(pw->pw_name);
    }
    return "Unknown";
#endif
}

size_t SystemUtils::getTotalMemory() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) {
        return memInfo.ullTotalPhys / (1024 * 1024);  // Convert to MB
    }
    return 0;
#else
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return (info.totalram * info.mem_unit) / (1024 * 1024);
    }
    return 0;
#endif
}

size_t SystemUtils::getAvailableMemory() {
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) {
        return memInfo.ullAvailPhys / (1024 * 1024);  // Convert to MB
    }
    return 0;
#else
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return (info.freeram * info.mem_unit) / (1024 * 1024);
    }
    return 0;
#endif
}

size_t SystemUtils::getCpuCoreCount() {
    return std::thread::hardware_concurrency();
}

std::string SystemUtils::getCpuModel() {
#ifdef _WIN32
    // Simplified version - would need more complex WMI queries for full CPU model
    return "Intel/AMD CPU";
#else
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line)) {
        if (line.find("model name") != std::string::npos) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string model = line.substr(colonPos + 1);
                // Trim leading spaces
                model.erase(0, model.find_first_not_of(" \t"));
                return model;
            }
        }
    }
    return "Unknown CPU";
#endif
}

std::optional<std::string> SystemUtils::getEnvironmentVariable(const std::string& name) {
    const char* value = std::getenv(name.c_str());
    if (value) {
        return std::string(value);
    }
    return std::nullopt;
}

bool SystemUtils::setEnvironmentVariable(const std::string& name, const std::string& value) {
#ifdef _WIN32
    // Convert to wide strings
    int nameLen = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, nullptr, 0);
    int valueLen = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);

    if (nameLen > 0 && valueLen > 0) {
        std::wstring wideName(nameLen - 1, L'\0');
        std::wstring wideValue(valueLen - 1, L'\0');

        MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, &wideName[0], nameLen);
        MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, &wideValue[0], valueLen);

        return SetEnvironmentVariableW(wideName.c_str(), wideValue.c_str()) != 0;
    }
    return false;
#else
    return setenv(name.c_str(), value.c_str(), 1) == 0;
#endif
}

std::unordered_map<std::string, std::string> SystemUtils::getAllEnvironmentVariables() {
    std::unordered_map<std::string, std::string> env;

#ifdef _WIN32
    LPWCH envStrings = GetEnvironmentStringsW();
    if (envStrings != nullptr) {
        LPWSTR var = envStrings;
        while (*var) {
            std::wstring wideEnvVar(var);

            // Convert wide string to narrow string
            int len = WideCharToMultiByte(CP_UTF8, 0, wideEnvVar.c_str(), -1, nullptr, 0, nullptr,
                                          nullptr);
            if (len > 0) {
                std::string envVar(len - 1, '\0');
                WideCharToMultiByte(CP_UTF8, 0, wideEnvVar.c_str(), -1, &envVar[0], len, nullptr,
                                    nullptr);

                size_t equalPos = envVar.find('=');
                if (equalPos != std::string::npos) {
                    std::string name = envVar.substr(0, equalPos);
                    std::string value = envVar.substr(equalPos + 1);
                    env[name] = value;
                }
            }
            var += wideEnvVar.length() + 1;
        }
        FreeEnvironmentStringsW(envStrings);
    }
#else
    extern char** environ;
    for (char** env_ptr = environ; *env_ptr != nullptr; ++env_ptr) {
        std::string envVar(*env_ptr);
        size_t equalPos = envVar.find('=');
        if (equalPos != std::string::npos) {
            std::string name = envVar.substr(0, equalPos);
            std::string value = envVar.substr(equalPos + 1);
            env[name] = value;
        }
    }
#endif

    return env;
}

bool SystemUtils::commandExists(const std::string& command) {
#ifdef _WIN32
    std::string cmd = "where " + command + " >nul 2>nul";
    return system(cmd.c_str()) == 0;
#else
    std::string cmd = "command -v " + command + " >/dev/null 2>&1";
    return system(cmd.c_str()) == 0;
#endif
}

std::string SystemUtils::executeSystemCommand(const std::string& command) {
    std::string result;
#ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif

    if (pipe) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe)) {
            result += buffer;
        }
#ifdef _WIN32
        _pclose(pipe);
#else
        pclose(pipe);
#endif
    }

    return result;
}

// Stub implementations for other methods
std::vector<CompilerInfo> SystemUtils::detectAvailableCompilers() {
    std::vector<CompilerInfo> compilers;

    std::vector<std::string> compilerNames = {"gcc", "g++", "clang", "clang++", "cl", "icc"};
    for (const auto& name : compilerNames) {
        CompilerInfo info;
        info.name = name;
        info.available = commandExists(name);
        if (info.available) {
            info.version = getToolVersion(name);
            info.path = findToolInPath(name).string();
        }
        compilers.push_back(info);
    }

    return compilers;
}

bool SystemUtils::isToolAvailable(const std::string& toolName) {
    return commandExists(toolName);
}

std::string SystemUtils::getToolVersion(const std::string& toolName) {
    std::string versionCmd = toolName + " --version";
    std::string output = executeSystemCommand(versionCmd);

    // Extract version from output (simplified)
    size_t pos = output.find("version");
    if (pos != std::string::npos) {
        size_t start = pos + 7;  // Skip "version"
        size_t end = output.find('\n', start);
        if (end == std::string::npos)
            end = output.length();
        return output.substr(start, end - start);
    }

    return "Unknown";
}

std::filesystem::path SystemUtils::findToolInPath(const std::string& toolName) {
    auto pathVar = getEnvironmentVariable("PATH");
    if (!pathVar)
        return {};

    std::string pathSeparator;
#ifdef _WIN32
    pathSeparator = ";";
    std::string extension = ".exe";
#else
    pathSeparator = ":";
    std::string extension = "";
#endif

    std::istringstream pathStream(*pathVar);
    std::string path;

    while (std::getline(pathStream, path, pathSeparator[0])) {
        std::filesystem::path fullPath = std::filesystem::path(path) / (toolName + extension);
        if (std::filesystem::exists(fullPath)) {
            return fullPath;
        }
    }

    return {};
}

// Default implementations for other methods to ensure compilation
bool SystemUtils::isCMakeAvailable() {
    return isToolAvailable("cmake");
}
bool SystemUtils::isNinjaAvailable() {
    return isToolAvailable("ninja");
}
bool SystemUtils::isMakeAvailable() {
    return isToolAvailable("make");
}
bool SystemUtils::isXMakeAvailable() {
    return isToolAvailable("xmake");
}
bool SystemUtils::isMesonAvailable() {
    return isToolAvailable("meson");
}
bool SystemUtils::isBazelAvailable() {
    return isToolAvailable("bazel");
}
bool SystemUtils::isVcpkgAvailable() {
    return isToolAvailable("vcpkg");
}
bool SystemUtils::isConanAvailable() {
    return isToolAvailable("conan");
}
bool SystemUtils::isGitAvailable() {
    return isToolAvailable("git");
}
bool SystemUtils::isPythonAvailable() {
    return isToolAvailable("python") || isToolAvailable("python3");
}
bool SystemUtils::isNodeJSAvailable() {
    return isToolAvailable("node");
}

std::string SystemUtils::getGitVersion() {
    return getToolVersion("git");
}
std::string SystemUtils::getPythonVersion() {
    if (isToolAvailable("python3"))
        return getToolVersion("python3");
    return getToolVersion("python");
}
std::string SystemUtils::getNodeJSVersion() {
    return getToolVersion("node");
}

// Additional stub implementations
std::vector<std::filesystem::path> SystemUtils::getPathDirectories() {
    std::vector<std::filesystem::path> paths;
    auto pathVar = getEnvironmentVariable("PATH");
    if (pathVar) {
        std::string pathSeparator;
#ifdef _WIN32
        pathSeparator = ";";
#else
        pathSeparator = ":";
#endif
        std::istringstream pathStream(*pathVar);
        std::string path;
        while (std::getline(pathStream, path, pathSeparator[0])) {
            if (!path.empty()) {
                paths.emplace_back(path);
            }
        }
    }
    return paths;
}

std::filesystem::path SystemUtils::getTempDirectory() {
    return std::filesystem::temp_directory_path();
}

std::filesystem::path SystemUtils::getCurrentDirectory() {
    return std::filesystem::current_path();
}

std::filesystem::path SystemUtils::getHomeDirectory() {
    auto homeVar = getEnvironmentVariable("HOME");
    if (homeVar) {
        return std::filesystem::path(*homeVar);
    }
#ifdef _WIN32
    auto userProfileVar = getEnvironmentVariable("USERPROFILE");
    if (userProfileVar) {
        return std::filesystem::path(*userProfileVar);
    }
#endif
    return {};
}

}  // namespace utils
