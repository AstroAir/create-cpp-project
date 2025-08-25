#pragma once

#include "../types/cli_enums.h"
#include <string>
#include <string_view>
#include <unordered_map>

// Localization helper class for multi-language support
class Localization {
public:
  // Initialize the localization system
  static void initialize();

  // Get localized string by key and language
  static std::string_view get(std::string_view key,
                              Language lang = Language::English);

  // Get and set current language
  static Language getCurrentLanguage();
  static void setCurrentLanguage(Language lang);

private:
  // Current language setting
  static Language s_currentLanguage;

  // String storage: key -> language -> localized string
  static std::unordered_map<std::string,
                            std::unordered_map<Language, std::string>>
      s_strings;

  // Load all language strings
  static void loadLanguageStrings();

  // Load strings for specific languages
  static void loadEnglishStrings();
  static void loadChineseStrings();
  static void loadSpanishStrings();
  static void loadJapaneseStrings();
  static void loadGermanStrings();
  static void loadFrenchStrings();
};
