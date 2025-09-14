#include "../external/http_client.h"
#include "../core/file_utils.h"
#include <spdlog/spdlog.h>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <regex>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace utils {

HttpClient::DownloadResult HttpClient::downloadFile(const std::string& url,
                                                   const std::filesystem::path& outputPath,
                                                   const ProgressCallback& progressCallback) {
    return downloadFileWithAuth(url, outputPath, std::nullopt, std::nullopt, {}, progressCallback);
}

HttpClient::DownloadResult HttpClient::downloadToMemory(const std::string& url,
                                                       std::string& content,
                                                       const ProgressCallback& progressCallback) {
    // Create temporary file for download
    std::filesystem::path tempFile = std::filesystem::temp_directory_path() / "cpp_scaffold_download.tmp";
    
    auto result = downloadFile(url, tempFile, progressCallback);
    if (result.success) {
        content = FileUtils::readFromFile(tempFile.string());
        std::filesystem::remove(tempFile);
    }
    
    return result;
}

HttpClient::DownloadResult HttpClient::downloadFileWithAuth(const std::string& url,
                                                           const std::filesystem::path& outputPath,
                                                           const std::optional<std::string>& username,
                                                           const std::optional<std::string>& password,
                                                           const std::map<std::string, std::string>& headers,
                                                           const ProgressCallback& progressCallback) {
    try {
        if (!isValidUrl(url)) {
            return {false, "Invalid URL format", 0, 0, {}};
        }

        // Ensure output directory exists
        std::filesystem::create_directories(outputPath.parent_path());

        // Try curl first, then wget as fallback
        if (hasCurl()) {
            return downloadWithCurl(url, outputPath, username, password, headers, progressCallback);
        } else if (hasWget()) {
            return downloadWithWget(url, outputPath, username, password);
        } else {
            return {false, "No HTTP client available (curl or wget required)", 0, 0, {}};
        }

    } catch (const std::exception& e) {
        spdlog::error("Error downloading file: {}", e.what());
        return {false, e.what(), 0, 0, {}};
    }
}

bool HttpClient::isUrlAccessible(const std::string& url) {
    if (!isValidUrl(url)) {
        return false;
    }

    if (hasCurl()) {
        std::string command = "curl -s -I \"" + url + "\"";
        return executeCommand(command);
    } else if (hasWget()) {
        std::string command = "wget --spider -q \"" + url + "\"";
        return executeCommand(command);
    }

    return false;
}

std::optional<size_t> HttpClient::getRemoteFileSize(const std::string& url) {
    if (!hasCurl()) {
        return std::nullopt;
    }

    std::string output;
    std::string command = "curl -sI \"" + url + "\" | grep -i content-length";
    
    if (executeCommand(command, &output)) {
        std::regex contentLengthRegex(R"(content-length:\s*(\d+))", std::regex_constants::icase);
        std::smatch match;
        
        if (std::regex_search(output, match, contentLengthRegex)) {
            try {
                return std::stoull(match[1].str());
            } catch (const std::exception&) {
                return std::nullopt;
            }
        }
    }

    return std::nullopt;
}

bool HttpClient::isValidUrl(const std::string& url) {
    if (url.empty()) return false;
    
    return url.substr(0, 7) == "http://" || 
           url.substr(0, 8) == "https://" ||
           url.substr(0, 6) == "ftp://";
}

std::string HttpClient::extractFilenameFromUrl(const std::string& url) {
    try {
        size_t lastSlash = url.find_last_of("/");
        if (lastSlash != std::string::npos && lastSlash < url.length() - 1) {
            std::string filename = url.substr(lastSlash + 1);
            
            // Remove query parameters
            size_t queryPos = filename.find("?");
            if (queryPos != std::string::npos) {
                filename = filename.substr(0, queryPos);
            }
            
            return filename.empty() ? "download" : filename;
        }
        return "download";
    } catch (const std::exception&) {
        return "download";
    }
}

std::string HttpClient::getMimeType(const std::string& url) {
    if (!hasCurl()) {
        return "application/octet-stream";
    }

    std::string output;
    std::string command = "curl -sI \"" + url + "\" | grep -i content-type";
    
    if (executeCommand(command, &output)) {
        std::regex contentTypeRegex(R"(content-type:\s*([^;\r\n]+))", std::regex_constants::icase);
        std::smatch match;
        
        if (std::regex_search(output, match, contentTypeRegex)) {
            return match[1].str();
        }
    }

    return "application/octet-stream";
}

HttpClient::DownloadResult HttpClient::downloadWithCurl(const std::string& url,
                                                       const std::filesystem::path& outputPath,
                                                       const std::optional<std::string>& username,
                                                       const std::optional<std::string>& password,
                                                       const std::map<std::string, std::string>& headers,
                                                       const ProgressCallback& progressCallback) {
    std::ostringstream command;
    command << "curl -L -o \"" << outputPath.string() << "\"";
    
    // Add authentication if provided
    if (username && password) {
        command << " -u \"" << *username << ":" << *password << "\"";
    }
    
    // Add custom headers
    for (const auto& header : headers) {
        command << " -H \"" << header.first << ": " << header.second << "\"";
    }
    
    // Add progress bar if no callback provided
    if (!progressCallback) {
        command << " --progress-bar";
    }
    
    command << " \"" << url << "\"";

    spdlog::info("Downloading {} to {}", url, outputPath.string());
    
    bool success = executeCommand(command.str());
    
    DownloadResult result;
    result.success = success && std::filesystem::exists(outputPath);
    
    if (result.success) {
        result.bytesDownloaded = std::filesystem::file_size(outputPath);
        spdlog::info("Download completed: {} bytes", result.bytesDownloaded);
    } else {
        result.errorMessage = "Download failed";
        spdlog::error("Download failed for URL: {}", url);
    }
    
    return result;
}

HttpClient::DownloadResult HttpClient::downloadWithWget(const std::string& url,
                                                       const std::filesystem::path& outputPath,
                                                       const std::optional<std::string>& username,
                                                       const std::optional<std::string>& password) {
    std::ostringstream command;
    command << "wget -O \"" << outputPath.string() << "\"";
    
    // Add authentication if provided
    if (username && password) {
        command << " --user=\"" << *username << "\" --password=\"" << *password << "\"";
    }
    
    command << " \"" << url << "\"";

    spdlog::info("Downloading {} to {} using wget", url, outputPath.string());
    
    bool success = executeCommand(command.str());
    
    DownloadResult result;
    result.success = success && std::filesystem::exists(outputPath);
    
    if (result.success) {
        result.bytesDownloaded = std::filesystem::file_size(outputPath);
        spdlog::info("Download completed: {} bytes", result.bytesDownloaded);
    } else {
        result.errorMessage = "Download failed";
        spdlog::error("Download failed for URL: {}", url);
    }
    
    return result;
}

bool HttpClient::hasCurl() {
    return executeCommand("curl --version");
}

bool HttpClient::hasWget() {
    return executeCommand("wget --version");
}

std::map<std::string, std::string> HttpClient::parseHeaders(const std::string& headerData) {
    std::map<std::string, std::string> headers;
    std::istringstream stream(headerData);
    std::string line;
    
    while (std::getline(stream, line)) {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            headers[key] = value;
        }
    }
    
    return headers;
}

bool HttpClient::executeCommand(const std::string& command, std::string* output) {
    try {
#ifdef _WIN32
        FILE* pipe = _popen(command.c_str(), "r");
#else
        FILE* pipe = popen(command.c_str(), "r");
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

} // namespace utils
