#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <optional>
#include <functional>

namespace utils {

// Archive extraction and validation utilities
class ArchiveUtils {
public:
    // Supported archive formats
    enum class ArchiveFormat {
        ZIP,
        TAR_GZ,
        TAR_BZ2,
        TAR_XZ,
        SEVEN_ZIP,
        TAR,
        UNKNOWN
    };

    // Extraction result structure
    struct ExtractionResult {
        bool success = false;
        std::string errorMessage;
        std::vector<std::string> extractedFiles;
        size_t totalFiles = 0;
        size_t totalBytes = 0;
    };

    // Progress callback function type: (current_file, total_files) -> bool
    // Return false to cancel extraction
    using ProgressCallback = std::function<bool(size_t current, size_t total)>;

    // Extract archive to target directory
    static ExtractionResult extractArchive(const std::filesystem::path& archivePath,
                                          const std::filesystem::path& targetPath,
                                          const ProgressCallback& progressCallback = nullptr);

    // Extract specific files from archive
    static ExtractionResult extractFiles(const std::filesystem::path& archivePath,
                                        const std::filesystem::path& targetPath,
                                        const std::vector<std::string>& filePatterns,
                                        const ProgressCallback& progressCallback = nullptr);

    // List contents of archive without extracting
    static std::vector<std::string> listArchiveContents(const std::filesystem::path& archivePath);

    // Validate archive integrity
    static bool validateArchive(const std::filesystem::path& archivePath);

    // Detect archive format from file extension or content
    static ArchiveFormat detectFormat(const std::filesystem::path& archivePath);

    // Get supported archive extensions
    static std::vector<std::string> getSupportedExtensions();

    // Check if file is a supported archive
    static bool isSupportedArchive(const std::filesystem::path& archivePath);

    // Get archive format name as string
    static std::string formatToString(ArchiveFormat format);

    // Create archive from directory (for backup/export functionality)
    static bool createArchive(const std::filesystem::path& sourcePath,
                             const std::filesystem::path& archivePath,
                             ArchiveFormat format = ArchiveFormat::ZIP);

private:
    // Format-specific extraction methods
    static ExtractionResult extractZip(const std::filesystem::path& archivePath,
                                      const std::filesystem::path& targetPath,
                                      const ProgressCallback& progressCallback);

    static ExtractionResult extractTarGz(const std::filesystem::path& archivePath,
                                        const std::filesystem::path& targetPath,
                                        const ProgressCallback& progressCallback);

    static ExtractionResult extractTarBz2(const std::filesystem::path& archivePath,
                                         const std::filesystem::path& targetPath,
                                         const ProgressCallback& progressCallback);

    static ExtractionResult extractTarXz(const std::filesystem::path& archivePath,
                                        const std::filesystem::path& targetPath,
                                        const ProgressCallback& progressCallback);

    static ExtractionResult extractSevenZip(const std::filesystem::path& archivePath,
                                           const std::filesystem::path& targetPath,
                                           const ProgressCallback& progressCallback);

    static ExtractionResult extractTar(const std::filesystem::path& archivePath,
                                      const std::filesystem::path& targetPath,
                                      const ProgressCallback& progressCallback);

    // Tool availability checks
    static bool hasUnzip();
    static bool hasTar();
    static bool hasSevenZip();

    // Execute system command with output capture
    static bool executeCommand(const std::string& command, std::string* output = nullptr);

    // Parse file list from command output
    static std::vector<std::string> parseFileList(const std::string& output, ArchiveFormat format);

    // Sanitize extracted file paths to prevent directory traversal
    static std::string sanitizePath(const std::string& path);

    // Check if path is safe for extraction
    static bool isSafePath(const std::string& path, const std::filesystem::path& targetDir);
};

} // namespace utils
