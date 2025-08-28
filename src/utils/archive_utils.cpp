#include "archive_utils.h"
#include "file_utils.h"
#include <spdlog/spdlog.h>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <regex>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace utils {

ArchiveUtils::ExtractionResult ArchiveUtils::extractArchive(const std::filesystem::path& archivePath,
                                                           const std::filesystem::path& targetPath,
                                                           const ProgressCallback& progressCallback) {
    try {
        if (!std::filesystem::exists(archivePath)) {
            return {false, "Archive file does not exist", {}, 0, 0};
        }

        if (!isSupportedArchive(archivePath)) {
            return {false, "Unsupported archive format", {}, 0, 0};
        }

        // Create target directory
        std::filesystem::create_directories(targetPath);

        ArchiveFormat format = detectFormat(archivePath);

        spdlog::info("Extracting {} archive: {} to {}",
                    formatToString(format), archivePath.string(), targetPath.string());

        switch (format) {
            case ArchiveFormat::ZIP:
                return extractZip(archivePath, targetPath, progressCallback);
            case ArchiveFormat::TAR_GZ:
                return extractTarGz(archivePath, targetPath, progressCallback);
            case ArchiveFormat::TAR_BZ2:
                return extractTarBz2(archivePath, targetPath, progressCallback);
            case ArchiveFormat::TAR_XZ:
                return extractTarXz(archivePath, targetPath, progressCallback);
            case ArchiveFormat::SEVEN_ZIP:
                return extractSevenZip(archivePath, targetPath, progressCallback);
            case ArchiveFormat::TAR:
                return extractTar(archivePath, targetPath, progressCallback);
            default:
                return {false, "Unsupported archive format", {}, 0, 0};
        }

    } catch (const std::exception& e) {
        spdlog::error("Error extracting archive: {}", e.what());
        return {false, e.what(), {}, 0, 0};
    }
}

std::vector<std::string> ArchiveUtils::listArchiveContents(const std::filesystem::path& archivePath) {
    if (!std::filesystem::exists(archivePath)) {
        return {};
    }

    ArchiveFormat format = detectFormat(archivePath);
    std::string command;
    std::string output;

    switch (format) {
        case ArchiveFormat::ZIP:
            if (hasUnzip()) {
                command = "unzip -l \"" + archivePath.string() + "\"";
            }
            break;
        case ArchiveFormat::TAR_GZ:
            if (hasTar()) {
                command = "tar -tzf \"" + archivePath.string() + "\"";
            }
            break;
        case ArchiveFormat::TAR_BZ2:
            if (hasTar()) {
                command = "tar -tjf \"" + archivePath.string() + "\"";
            }
            break;
        case ArchiveFormat::TAR_XZ:
            if (hasTar()) {
                command = "tar -tJf \"" + archivePath.string() + "\"";
            }
            break;
        case ArchiveFormat::TAR:
            if (hasTar()) {
                command = "tar -tf \"" + archivePath.string() + "\"";
            }
            break;
        case ArchiveFormat::SEVEN_ZIP:
            if (hasSevenZip()) {
                command = "7z l \"" + archivePath.string() + "\"";
            }
            break;
        default:
            return {};
    }

    if (!command.empty() && executeCommand(command, &output)) {
        return parseFileList(output, format);
    }

    return {};
}

bool ArchiveUtils::validateArchive(const std::filesystem::path& archivePath) {
    if (!std::filesystem::exists(archivePath)) {
        return false;
    }

    ArchiveFormat format = detectFormat(archivePath);
    std::string command;

    switch (format) {
        case ArchiveFormat::ZIP:
            if (hasUnzip()) {
                command = "unzip -t \"" + archivePath.string() + "\"";
            }
            break;
        case ArchiveFormat::TAR_GZ:
        case ArchiveFormat::TAR_BZ2:
        case ArchiveFormat::TAR_XZ:
        case ArchiveFormat::TAR:
            if (hasTar()) {
                command = "tar -tf \"" + archivePath.string() + "\" > /dev/null";
            }
            break;
        case ArchiveFormat::SEVEN_ZIP:
            if (hasSevenZip()) {
                command = "7z t \"" + archivePath.string() + "\"";
            }
            break;
        default:
            return false;
    }

    return !command.empty() && executeCommand(command);
}

ArchiveUtils::ArchiveFormat ArchiveUtils::detectFormat(const std::filesystem::path& archivePath) {
    std::string extension = archivePath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension == ".zip") {
        return ArchiveFormat::ZIP;
    } else if (extension == ".gz" && archivePath.stem().extension() == ".tar") {
        return ArchiveFormat::TAR_GZ;
    } else if (extension == ".bz2" && archivePath.stem().extension() == ".tar") {
        return ArchiveFormat::TAR_BZ2;
    } else if (extension == ".xz" && archivePath.stem().extension() == ".tar") {
        return ArchiveFormat::TAR_XZ;
    } else if (extension == ".7z") {
        return ArchiveFormat::SEVEN_ZIP;
    } else if (extension == ".tar") {
        return ArchiveFormat::TAR;
    }

    return ArchiveFormat::UNKNOWN;
}

std::vector<std::string> ArchiveUtils::getSupportedExtensions() {
    return {".zip", ".tar.gz", ".tar.bz2", ".tar.xz", ".7z", ".tar"};
}

bool ArchiveUtils::isSupportedArchive(const std::filesystem::path& archivePath) {
    return detectFormat(archivePath) != ArchiveFormat::UNKNOWN;
}

std::string ArchiveUtils::formatToString(ArchiveFormat format) {
    switch (format) {
        case ArchiveFormat::ZIP: return "ZIP";
        case ArchiveFormat::TAR_GZ: return "TAR.GZ";
        case ArchiveFormat::TAR_BZ2: return "TAR.BZ2";
        case ArchiveFormat::TAR_XZ: return "TAR.XZ";
        case ArchiveFormat::SEVEN_ZIP: return "7Z";
        case ArchiveFormat::TAR: return "TAR";
        default: return "UNKNOWN";
    }
}

ArchiveUtils::ExtractionResult ArchiveUtils::extractZip(const std::filesystem::path& archivePath,
                                                       const std::filesystem::path& targetPath,
                                                       const ProgressCallback& progressCallback) {
    (void)progressCallback; // TODO: Implement progress callback support
    if (!hasUnzip()) {
        return {false, "unzip command not available", {}, 0, 0};
    }

    std::string command = "unzip -o \"" + archivePath.string() + "\" -d \"" + targetPath.string() + "\"";

    if (executeCommand(command)) {
        ExtractionResult result;
        result.success = true;

        // Get list of extracted files
        auto contents = listArchiveContents(archivePath);
        result.extractedFiles = contents;
        result.totalFiles = contents.size();

        spdlog::info("Successfully extracted {} files from ZIP archive", result.totalFiles);
        return result;
    }

    return {false, "Failed to extract ZIP archive", {}, 0, 0};
}

ArchiveUtils::ExtractionResult ArchiveUtils::extractTarGz(const std::filesystem::path& archivePath,
                                                         const std::filesystem::path& targetPath,
                                                         const ProgressCallback& progressCallback) {
    (void)progressCallback; // TODO: Implement progress callback support
    if (!hasTar()) {
        return {false, "tar command not available", {}, 0, 0};
    }

    std::string command = "tar -xzf \"" + archivePath.string() + "\" -C \"" + targetPath.string() + "\"";

    if (executeCommand(command)) {
        ExtractionResult result;
        result.success = true;

        auto contents = listArchiveContents(archivePath);
        result.extractedFiles = contents;
        result.totalFiles = contents.size();

        spdlog::info("Successfully extracted {} files from TAR.GZ archive", result.totalFiles);
        return result;
    }

    return {false, "Failed to extract TAR.GZ archive", {}, 0, 0};
}

ArchiveUtils::ExtractionResult ArchiveUtils::extractTarBz2(const std::filesystem::path& archivePath,
                                                          const std::filesystem::path& targetPath,
                                                          const ProgressCallback& progressCallback) {
    (void)progressCallback; // TODO: Implement progress callback support
    if (!hasTar()) {
        return {false, "tar command not available", {}, 0, 0};
    }

    std::string command = "tar -xjf \"" + archivePath.string() + "\" -C \"" + targetPath.string() + "\"";

    if (executeCommand(command)) {
        ExtractionResult result;
        result.success = true;

        auto contents = listArchiveContents(archivePath);
        result.extractedFiles = contents;
        result.totalFiles = contents.size();

        spdlog::info("Successfully extracted {} files from TAR.BZ2 archive", result.totalFiles);
        return result;
    }

    return {false, "Failed to extract TAR.BZ2 archive", {}, 0, 0};
}

ArchiveUtils::ExtractionResult ArchiveUtils::extractTarXz(const std::filesystem::path& archivePath,
                                                         const std::filesystem::path& targetPath,
                                                         const ProgressCallback& progressCallback) {
    (void)progressCallback; // TODO: Implement progress callback support
    if (!hasTar()) {
        return {false, "tar command not available", {}, 0, 0};
    }

    std::string command = "tar -xJf \"" + archivePath.string() + "\" -C \"" + targetPath.string() + "\"";

    if (executeCommand(command)) {
        ExtractionResult result;
        result.success = true;

        auto contents = listArchiveContents(archivePath);
        result.extractedFiles = contents;
        result.totalFiles = contents.size();

        spdlog::info("Successfully extracted {} files from TAR.XZ archive", result.totalFiles);
        return result;
    }

    return {false, "Failed to extract TAR.XZ archive", {}, 0, 0};
}

ArchiveUtils::ExtractionResult ArchiveUtils::extractSevenZip(const std::filesystem::path& archivePath,
                                                            const std::filesystem::path& targetPath,
                                                            const ProgressCallback& progressCallback) {
    (void)progressCallback; // TODO: Implement progress callback support
    if (!hasSevenZip()) {
        return {false, "7z command not available", {}, 0, 0};
    }

    std::string command = "7z x \"" + archivePath.string() + "\" -o\"" + targetPath.string() + "\" -y";

    if (executeCommand(command)) {
        ExtractionResult result;
        result.success = true;

        auto contents = listArchiveContents(archivePath);
        result.extractedFiles = contents;
        result.totalFiles = contents.size();

        spdlog::info("Successfully extracted {} files from 7Z archive", result.totalFiles);
        return result;
    }

    return {false, "Failed to extract 7Z archive", {}, 0, 0};
}

ArchiveUtils::ExtractionResult ArchiveUtils::extractTar(const std::filesystem::path& archivePath,
                                                       const std::filesystem::path& targetPath,
                                                       const ProgressCallback& progressCallback) {
    (void)progressCallback; // TODO: Implement progress callback support
    if (!hasTar()) {
        return {false, "tar command not available", {}, 0, 0};
    }

    std::string command = "tar -xf \"" + archivePath.string() + "\" -C \"" + targetPath.string() + "\"";

    if (executeCommand(command)) {
        ExtractionResult result;
        result.success = true;

        auto contents = listArchiveContents(archivePath);
        result.extractedFiles = contents;
        result.totalFiles = contents.size();

        spdlog::info("Successfully extracted {} files from TAR archive", result.totalFiles);
        return result;
    }

    return {false, "Failed to extract TAR archive", {}, 0, 0};
}

bool ArchiveUtils::hasUnzip() {
    return executeCommand("unzip -v");
}

bool ArchiveUtils::hasTar() {
    return executeCommand("tar --version");
}

bool ArchiveUtils::hasSevenZip() {
    return executeCommand("7z");
}

bool ArchiveUtils::executeCommand(const std::string& command, std::string* output) {
    try {
#ifdef _WIN32
        FILE* pipe = _popen((command + " 2>NUL").c_str(), "r");
#else
        FILE* pipe = popen((command + " 2>/dev/null").c_str(), "r");
#endif

        if (!pipe) {
            return false;
        }

        if (output) {
            char buffer[128];
            std::ostringstream result;
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result << buffer;
            }
            *output = result.str();
        }

#ifdef _WIN32
        int exitCode = _pclose(pipe);
#else
        int exitCode = pclose(pipe);
#endif

        return exitCode == 0;

    } catch (const std::exception& e) {
        spdlog::error("Error executing command: {}", e.what());
        return false;
    }
}

std::vector<std::string> ArchiveUtils::parseFileList(const std::string& output, ArchiveFormat format) {
    std::vector<std::string> files;
    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line)) {
        // Simple parsing - extract filenames based on format
        if (format == ArchiveFormat::ZIP) {
            // Skip header lines and extract filename from unzip -l output
            if (line.find("Archive:") == std::string::npos &&
                line.find("Length") == std::string::npos &&
                line.find("----") == std::string::npos &&
                !line.empty()) {

                // Extract filename (last part after spaces)
                size_t lastSpace = line.find_last_of(' ');
                if (lastSpace != std::string::npos && lastSpace < line.length() - 1) {
                    files.push_back(line.substr(lastSpace + 1));
                }
            }
        } else {
            // For tar formats, each line is typically a filename
            if (!line.empty() && line.back() != '/') {
                files.push_back(line);
            }
        }
    }

    return files;
}

} // namespace utils
