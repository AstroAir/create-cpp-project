#include "string_utils.h"

#include <algorithm>

namespace utils {
std::string StringUtils::toLower(const std::string &str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}

std::string StringUtils::toUpper(const std::string &str) {
  std::string result = str;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return result;
}

std::string StringUtils::replace(const std::string &str,
                                 const std::string &from,
                                 const std::string &to) {
  // Handle edge case: empty 'from' string would cause infinite loop
  if (from.empty()) {
    return str;
  }

  std::string result = str;
  size_t pos = 0;
  while ((pos = result.find(from, pos)) != std::string::npos) {
    result.replace(pos, from.length(), to);
    pos += to.length();
  }
  return result;
}

std::vector<std::string> StringUtils::split(const std::string &str,
                                            char delimiter) {
  std::vector<std::string> tokens;

  // Handle empty string case
  if (str.empty()) {
    tokens.push_back("");
    return tokens;
  }

  size_t start = 0;
  size_t end = 0;

  while ((end = str.find(delimiter, start)) != std::string::npos) {
    tokens.push_back(str.substr(start, end - start));
    start = end + 1;
  }

  // Add the last token (or empty string if string ends with delimiter)
  tokens.push_back(str.substr(start));

  return tokens;
}

std::string StringUtils::trim(const std::string &str) {
  auto start = str.begin();
  while (start != str.end() && std::isspace(*start)) {
    start++;
  }

  auto end = str.end();
  do {
    end--;
  } while (std::distance(start, end) > 0 && std::isspace(*end));

  return std::string(start, end + 1);
}

bool StringUtils::startsWith(const std::string &str,
                             const std::string &prefix) {
  return str.size() >= prefix.size() &&
         str.compare(0, prefix.size(), prefix) == 0;
}

bool StringUtils::endsWith(const std::string &str, const std::string &suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string StringUtils::join(const std::vector<std::string> &strings, const std::string &delimiter) {
  if (strings.empty()) {
    return "";
  }

  std::string result = strings[0];
  for (size_t i = 1; i < strings.size(); ++i) {
    result += delimiter + strings[i];
  }
  return result;
}
} // namespace utils