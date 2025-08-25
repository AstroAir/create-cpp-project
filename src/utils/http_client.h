#pragma once

#include <string>
#include <filesystem>
#include <optional>
#include <functional>
#include <map>

namespace utils {

// HTTP client utilities for downloading files and archives
class HttpClient {
public:
    // Progress callback function type: (downloaded_bytes, total_bytes) -> bool
    // Return false to cancel download
    using ProgressCallback = std::function<bool(size_t downloaded, size_t total)>;

    // Download result structure
    struct DownloadResult {
        bool success = false;
        std::string errorMessage;
        size_t bytesDownloaded = 0;
        int httpStatusCode = 0;
        std::map<std::string, std::string> headers;
    };

    // Download file from URL to local path
    static DownloadResult downloadFile(const std::string& url,
                                     const std::filesystem::path& outputPath,
                                     const ProgressCallback& progressCallback = nullptr);

    // Download file to memory (for small files)
    static DownloadResult downloadToMemory(const std::string& url,
                                         std::string& content,
                                         const ProgressCallback& progressCallback = nullptr);

    // Download with authentication
    static DownloadResult downloadFileWithAuth(const std::string& url,
                                              const std::filesystem::path& outputPath,
                                              const std::optional<std::string>& username = std::nullopt,
                                              const std::optional<std::string>& password = std::nullopt,
                                              const std::map<std::string, std::string>& headers = {},
                                              const ProgressCallback& progressCallback = nullptr);

    // Check if URL is accessible (HEAD request)
    static bool isUrlAccessible(const std::string& url);

    // Get file size from URL without downloading
    static std::optional<size_t> getRemoteFileSize(const std::string& url);

    // Validate URL format
    static bool isValidUrl(const std::string& url);

    // Extract filename from URL
    static std::string extractFilenameFromUrl(const std::string& url);

    // Get MIME type from URL/headers
    static std::string getMimeType(const std::string& url);

private:
    // Internal download implementation using system tools
    static DownloadResult downloadWithCurl(const std::string& url,
                                          const std::filesystem::path& outputPath,
                                          const std::optional<std::string>& username = std::nullopt,
                                          const std::optional<std::string>& password = std::nullopt,
                                          const std::map<std::string, std::string>& headers = {},
                                          const ProgressCallback& progressCallback = nullptr);

    // Fallback download implementation using wget
    static DownloadResult downloadWithWget(const std::string& url,
                                          const std::filesystem::path& outputPath,
                                          const std::optional<std::string>& username = std::nullopt,
                                          const std::optional<std::string>& password = std::nullopt);

    // Check if curl is available
    static bool hasCurl();

    // Check if wget is available
    static bool hasWget();

    // Parse HTTP headers from curl output
    static std::map<std::string, std::string> parseHeaders(const std::string& headerData);

    // Execute system command and capture output
    static bool executeCommand(const std::string& command, std::string* output = nullptr);
};

} // namespace utils
