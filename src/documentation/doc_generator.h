#pragma once

#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <optional>
#include <functional>
#include <memory>

namespace documentation {

// Forward declarations
class APIDocExtractor;

// API documentation structures
struct ClassInfo {
    std::string name;
    std::string description;
    std::string namespace_;
    std::vector<std::string> baseClasses;
    std::vector<std::string> methods;
    std::vector<std::string> members;
    std::map<std::string, std::string> methodDocs;
    std::map<std::string, std::string> memberDocs;
};

struct FunctionInfo {
    std::string name;
    std::string description;
    std::string returnType;
    std::vector<std::pair<std::string, std::string>> parameters;
    std::string namespace_;
    bool isStatic{false};
    bool isConst{false};
};

struct NamespaceInfo {
    std::string name;
    std::string description;
    std::vector<ClassInfo> classes;
    std::vector<FunctionInfo> functions;
    std::vector<std::string> enums;
    std::vector<std::string> typedefs;
};

// Documentation formats
enum class DocFormat {
    Markdown,
    HTML,
    PDF,
    LaTeX,
    Sphinx,
    GitBook,
    Doxygen,
    Custom
};

// Documentation types
enum class DocType {
    API,
    User,
    Developer,
    Tutorial,
    Reference,
    Changelog,
    Contributing,
    License,
    README
};

// Documentation configuration
struct DocConfig {
    std::string projectName;
    std::string projectVersion{"1.0.0"};
    std::string projectDescription;
    std::string author;
    std::string email;
    std::string license{"MIT"};
    std::string repository;
    std::string website;

    std::vector<DocFormat> outputFormats{DocFormat::Markdown, DocFormat::HTML};
    std::vector<DocType> documentTypes{DocType::API, DocType::User, DocType::README};

    std::string outputDirectory{"docs"};
    std::string sourceDirectory{"src"};
    std::string templateDirectory{"templates"};

    bool generateIndex{true};
    bool generateTOC{true};
    bool generateSearchIndex{true};
    bool includeSourceCode{true};
    bool includeExamples{true};
    bool includeDiagrams{false};

    std::string theme{"default"};
    std::map<std::string, std::string> customSettings;
};

// Documentation generator
class DocGenerator {
public:
    DocGenerator(const DocConfig& config = DocConfig{});

    // Configuration
    void setConfig(const DocConfig& config);
    DocConfig getConfig() const;

    // Generation
    bool generateDocumentation(const std::filesystem::path& projectPath);
    bool generateSpecificDoc(DocType type, const std::filesystem::path& projectPath);
    bool generateForFormat(DocFormat format, const std::filesystem::path& projectPath);

    // Content generation
    bool generateREADME(const std::filesystem::path& projectPath);
    bool generateAPIDocumentation(const std::filesystem::path& projectPath);
    bool generateUserGuide(const std::filesystem::path& projectPath);
    bool generateDeveloperGuide(const std::filesystem::path& projectPath);
    bool generateTutorials(const std::filesystem::path& projectPath);
    bool generateChangelog(const std::filesystem::path& projectPath);
    bool generateContributingGuide(const std::filesystem::path& projectPath);
    bool generateLicense(const std::filesystem::path& projectPath);

    // Format conversion
    bool convertToHTML(const std::filesystem::path& markdownFile, const std::filesystem::path& outputFile);
    bool convertToPDF(const std::filesystem::path& markdownFile, const std::filesystem::path& outputFile);
    bool convertToLaTeX(const std::filesystem::path& markdownFile, const std::filesystem::path& outputFile);

    // Template management
    bool loadTemplate(const std::string& templateName, const std::filesystem::path& templatePath);
    bool saveTemplate(const std::string& templateName, const std::filesystem::path& templatePath);
    std::vector<std::string> listAvailableTemplates() const;

    // Doxygen integration
    bool generateDoxygenConfig(const std::filesystem::path& projectPath);
    bool runDoxygenGeneration(const std::filesystem::path& projectPath);
    bool setupDoxygenTheme(const std::filesystem::path& projectPath, const std::string& theme);

    // Validation
    bool validateDocumentation(const std::filesystem::path& projectPath);
    std::vector<std::string> getValidationErrors() const;

private:
    DocConfig m_config;
    std::map<std::string, std::string> m_templates;
    std::vector<std::string> m_validationErrors;

    // Content generators
    std::string generateREADMEContent();
    std::string generateAPIContent(const std::filesystem::path& sourceDir);
    std::string generateAPIIndexContent(const APIDocExtractor& extractor);
    std::string generateNamespaceDocumentation(const NamespaceInfo& ns);
    std::string generateClassDocumentation(const ClassInfo& cls);
    std::string generateUserGuideContent();
    std::string generateDeveloperGuideContent();
    std::string generateTutorialContent();
    std::string generateChangelogContent();
    std::string generateContributingContent();
    std::string generateLicenseContent();

    // Format processors
    std::string processMarkdown(const std::string& content);
    std::string processHTML(const std::string& content);
    std::string processLaTeX(const std::string& content);

    // Utility methods
    std::string replaceVariables(const std::string& content) const;
    std::string generateTableOfContents(const std::string& content) const;
    std::string generateIndex(const std::vector<std::string>& files) const;
    bool createOutputDirectory(const std::filesystem::path& path);

    // Doxygen helpers
    std::string generateDoxyfileContent() const;
    std::string getDoxygenThemeConfig(const std::string& theme) const;
    bool hasDoxygenInstalled() const;
};

// API documentation extractor
class APIDocExtractor {
public:

    APIDocExtractor();

    // Extraction
    bool extractFromDirectory(const std::filesystem::path& sourceDir);
    bool extractFromFile(const std::filesystem::path& sourceFile);

    // Access
    std::vector<NamespaceInfo> getNamespaces() const;
    std::vector<ClassInfo> getClasses() const;
    std::vector<FunctionInfo> getFunctions() const;

    // Generation
    std::string generateMarkdownAPI() const;
    std::string generateHTMLAPI() const;
    std::string generateDoxygenConfig() const;

private:
    std::vector<NamespaceInfo> m_namespaces;
    std::vector<ClassInfo> m_classes;
    std::vector<FunctionInfo> m_functions;

    // Parsing helpers
    bool parseHeaderFile(const std::filesystem::path& headerFile);
    bool parseSourceFile(const std::filesystem::path& sourceFile);
    ClassInfo extractClassInfo(const std::string& classDeclaration);
    FunctionInfo extractFunctionInfo(const std::string& functionDeclaration);
    std::string extractDocComment(const std::string& code, size_t position);

    // Formatting helpers
    std::string formatClassMarkdown(const ClassInfo& classInfo) const;
    std::string formatFunctionMarkdown(const FunctionInfo& functionInfo) const;
    std::string formatParameterList(const std::vector<std::pair<std::string, std::string>>& parameters) const;
};

// Documentation template system
class DocTemplateSystem {
public:
    DocTemplateSystem();

    // Template management
    bool loadTemplate(const std::string& name, const std::filesystem::path& path);
    bool saveTemplate(const std::string& name, const std::filesystem::path& path);
    std::string getTemplate(const std::string& name) const;
    std::vector<std::string> listTemplates() const;

    // Template processing
    std::string processTemplate(const std::string& templateName, const std::map<std::string, std::string>& variables) const;
    std::string processContent(const std::string& content, const std::map<std::string, std::string>& variables) const;

    // Built-in templates
    void initializeBuiltinTemplates();

private:
    std::map<std::string, std::string> m_templates;

    // Template processors
    std::string replaceVariables(const std::string& content, const std::map<std::string, std::string>& variables) const;
    std::string processIncludes(const std::string& content) const;
    std::string processConditionals(const std::string& content, const std::map<std::string, std::string>& variables) const;
    std::string processLoops(const std::string& content, const std::map<std::string, std::string>& variables) const;
};

// Documentation builder for CI/CD integration
class DocBuilder {
public:
    DocBuilder(const DocConfig& config);

    // Build process
    bool build(const std::filesystem::path& projectPath);
    bool buildAndDeploy(const std::filesystem::path& projectPath, const std::string& deployTarget);

    // CI/CD integration
    std::string generateGitHubPagesWorkflow() const;
    std::string generateGitLabPagesConfig() const;
    std::string generateNetlifyConfig() const;
    std::string generateVercelConfig() const;

    // Build configuration
    void setOutputDirectory(const std::filesystem::path& path);
    void setBaseURL(const std::string& url);
    void setTheme(const std::string& theme);

    // Deployment
    bool deployToGitHubPages(const std::filesystem::path& docsPath);
    bool deployToNetlify(const std::filesystem::path& docsPath, const std::string& siteId);
    bool deployToVercel(const std::filesystem::path& docsPath, const std::string& projectId);

private:
    DocConfig m_config;
    std::filesystem::path m_outputDirectory;
    std::string m_baseURL;
    std::string m_theme;

    // Build helpers
    bool preprocessFiles(const std::filesystem::path& sourceDir);
    bool generateStaticSite(const std::filesystem::path& sourceDir);
    bool optimizeAssets(const std::filesystem::path& outputDir);
    bool generateSitemap(const std::filesystem::path& outputDir);
};

// Documentation utilities
namespace doc_utils {
    // Format detection
    DocFormat detectFormat(const std::filesystem::path& filePath);
    std::string formatToString(DocFormat format);
    std::optional<DocFormat> stringToFormat(const std::string& name);

    // Content utilities
    std::string extractTitle(const std::string& content);
    std::vector<std::string> extractHeaders(const std::string& content);
    std::string generateTOC(const std::string& content);
    std::string sanitizeFilename(const std::string& filename);

    // Markdown utilities
    std::string markdownToHTML(const std::string& markdown);
    std::string htmlToMarkdown(const std::string& html);
    std::string escapeMarkdown(const std::string& text);
    std::string unescapeMarkdown(const std::string& text);

    // File utilities
    bool isDocumentationFile(const std::filesystem::path& filePath);
    std::vector<std::filesystem::path> findDocumentationFiles(const std::filesystem::path& directory);
    bool copyAssets(const std::filesystem::path& sourceDir, const std::filesystem::path& targetDir);

    // Validation
    bool validateMarkdown(const std::string& content);
    bool validateHTML(const std::string& content);
    std::vector<std::string> checkLinks(const std::string& content, const std::filesystem::path& basePath);

    // License utilities
    std::string getLicenseText(const std::string& licenseName);
    std::vector<std::string> getAvailableLicenses();
    bool isValidLicense(const std::string& licenseName);
}

} // namespace documentation
