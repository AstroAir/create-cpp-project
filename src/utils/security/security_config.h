#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

namespace utils {

// Security hardening levels
enum class SecurityLevel {
    None,       // No security hardening
    Basic,      // Basic security measures
    Enhanced,   // Enhanced security with performance trade-offs
    Paranoid    // Maximum security, significant performance impact
};

// Static analysis tools
enum class StaticAnalyzer {
    None,
    ClangStaticAnalyzer,
    CppCheck,
    PVSStudio,
    SonarQube,
    Clazy,
    IncludeWhatYouUse
};

// Runtime sanitizers
enum class Sanitizer {
    None,
    AddressSanitizer,      // ASan - memory errors
    ThreadSanitizer,       // TSan - thread safety
    UndefinedBehaviorSanitizer, // UBSan - undefined behavior
    MemorySanitizer,       // MSan - uninitialized memory
    LeakSanitizer,         // LSan - memory leaks
    ControlFlowIntegrity   // CFI - control flow integrity
};

// Compiler security flags
enum class SecurityFlag {
    StackProtector,        // -fstack-protector-strong
    Fortify,              // -D_FORTIFY_SOURCE=2
    RelRO,                // -Wl,-z,relro
    NowBinding,           // -Wl,-z,now
    NoExecutableStack,    // -Wl,-z,noexecstack
    PieExecutable,        // -fPIE -pie
    ControlFlowGuard,     // /guard:cf (MSVC)
    ShadowStack,          // -fcf-protection=full
    ReturnAddressAuth,    // ARM pointer authentication
    BranchTargetId        // ARM branch target identification
};

// Security configuration structure
struct SecurityConfig {
    SecurityLevel level = SecurityLevel::Basic;

    // Static analysis configuration
    std::vector<StaticAnalyzer> staticAnalyzers;
    bool enableStaticAnalysisInCI = true;
    bool failBuildOnStaticAnalysisErrors = false;
    std::unordered_map<StaticAnalyzer, std::string> analyzerConfigs;

    // Runtime sanitizers configuration
    std::vector<Sanitizer> sanitizers;
    bool enableSanitizersInDebug = true;
    bool enableSanitizersInRelease = false;
    std::unordered_map<Sanitizer, std::string> sanitizerOptions;

    // Compiler security flags
    std::vector<SecurityFlag> securityFlags;
    bool enableSecurityFlagsInRelease = true;
    bool enableSecurityFlagsInDebug = true;

    // Dependency security
    bool enableDependencyScanning = true;
    bool enableVulnerabilityChecks = true;
    std::vector<std::string> trustedSources;
    std::vector<std::string> blockedPackages;

    // Code quality and security practices
    bool enforceCodeSigning = false;
    bool requireSecureHeaders = true;
    bool enableWarningsAsErrors = false;
    bool enableExtraWarnings = true;

    // Security testing
    bool enableFuzzTesting = false;
    bool enablePenetrationTesting = false;
    std::string fuzzTestingFramework = "libfuzzer";

    // Secrets and credentials
    bool scanForSecrets = true;
    bool enableSecretsDetection = true;
    std::vector<std::string> secretsPatterns;

    // Custom security configurations
    std::unordered_map<std::string, std::string> customConfigs;
};

// Security configuration manager
class SecurityConfigManager {
public:
    SecurityConfigManager();

    // Configuration management
    SecurityConfig getDefaultConfig(SecurityLevel level) const;
    SecurityConfig createCustomConfig(const std::vector<std::string>& options) const;
    bool validateConfig(const SecurityConfig& config) const;

    // Build system integration
    std::string generateCMakeSecurityConfig(const SecurityConfig& config) const;
    std::string generateMesonSecurityConfig(const SecurityConfig& config) const;
    std::string generateBazelSecurityConfig(const SecurityConfig& config) const;

    // Static analyzer integration
    std::string generateClangStaticAnalyzerConfig(const SecurityConfig& config) const;
    std::string generateCppCheckConfig(const SecurityConfig& config) const;
    std::string generatePVSStudioConfig(const SecurityConfig& config) const;

    // Sanitizer configuration
    std::string generateSanitizerFlags(const SecurityConfig& config, bool isDebug) const;
    std::string generateSanitizerCMakeConfig(const SecurityConfig& config) const;

    // Security flags generation
    std::string generateSecurityFlags(const SecurityConfig& config, const std::string& compiler) const;
    std::string generateLinkerSecurityFlags(const SecurityConfig& config) const;

    // CI/CD integration
    std::string generateGitHubActionsSecurityConfig(const SecurityConfig& config) const;
    std::string generateGitLabCISecurityConfig(const SecurityConfig& config) const;

    // Documentation generation
    std::string generateSecurityDocumentation(const SecurityConfig& config) const;
    std::string generateSecurityChecklist(const SecurityConfig& config) const;

private:
    void initializeDefaultConfigs();
    std::string getAnalyzerCommand(StaticAnalyzer analyzer) const;
    std::string getSanitizerFlag(Sanitizer sanitizer) const;
    std::string getSecurityFlagString(SecurityFlag flag, const std::string& compiler) const;
    std::string getAnalyzerDescription(StaticAnalyzer analyzer) const;
    std::string getSanitizerDescription(Sanitizer sanitizer) const;
    std::string getSecurityFlagDescription(SecurityFlag flag) const;

    std::unordered_map<SecurityLevel, SecurityConfig> defaultConfigs_;
    std::unordered_map<StaticAnalyzer, std::string> analyzerCommands_;
    std::unordered_map<Sanitizer, std::string> sanitizerFlags_;
};

// Utility functions
std::string to_string(SecurityLevel level);
std::string to_string(StaticAnalyzer analyzer);
std::string to_string(Sanitizer sanitizer);
std::string to_string(SecurityFlag flag);

std::optional<SecurityLevel> to_security_level(const std::string& str);
std::optional<StaticAnalyzer> to_static_analyzer(const std::string& str);
std::optional<Sanitizer> to_sanitizer(const std::string& str);
std::optional<SecurityFlag> to_security_flag(const std::string& str);

} // namespace utils
