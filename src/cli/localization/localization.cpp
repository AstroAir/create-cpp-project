#include "localization.h"

// Initialize static members
Language Localization::s_currentLanguage = Language::English;
std::unordered_map<std::string, std::unordered_map<Language, std::string>> Localization::s_strings;

// Initialize the localization system
void Localization::initialize() {
    loadLanguageStrings();
}

// Get localized string by key and language
std::string_view Localization::get(std::string_view key, Language lang) {
    auto langStr = std::string(key);
    auto langIt = s_strings.find(langStr);

    if (langIt != s_strings.end()) {
        auto strIt = langIt->second.find(lang);
        if (strIt != langIt->second.end()) {
            return strIt->second;
        }

        // Try using English as fallback
        if (lang != Language::English) {
            strIt = langIt->second.find(Language::English);
            if (strIt != langIt->second.end()) {
                return strIt->second;
            }
        }
    }

    // Return key as fallback
    return key;
}

// Get current language
Language Localization::getCurrentLanguage() {
    return s_currentLanguage;
}

// Set current language
void Localization::setCurrentLanguage(Language lang) {
    s_currentLanguage = lang;
}

// Load all language strings
void Localization::loadLanguageStrings() {
    loadEnglishStrings();
    loadChineseStrings();
    loadSpanishStrings();
    loadJapaneseStrings();
    loadGermanStrings();
    loadFrenchStrings();
}

// Load English strings (default)
void Localization::loadEnglishStrings() {
    s_strings["welcomeMessage"][Language::English] =
            "Welcome to CPP-Scaffold! Let's create your C++ project.";
    s_strings["projectName"][Language::English] = "Project name";
    s_strings["projectNameEmpty"][Language::English] = "Project name cannot be empty.";
    s_strings["selectProjectType"][Language::English] = "Select project type";
    s_strings["selectBuildSystem"][Language::English] = "Select build system";
    s_strings["selectPackageManager"][Language::English] = "Select package manager";
    s_strings["includeTests"][Language::English] = "Include unit testing framework?";
    s_strings["selectTestFramework"][Language::English] = "Select test framework";
    s_strings["includeDocumentation"][Language::English] =
            "Include project documentation configuration?";
    s_strings["includeCodeStyleTools"][Language::English] =
            "Include code style and static analysis tools?";
    s_strings["selectEditors"][Language::English] = "Select editors to configure support for";
    s_strings["selectCISystems"][Language::English] = "Select CI/CD systems to configure";
    s_strings["initGit"][Language::English] = "Initialize Git repository?";
    s_strings["invalidSelection"][Language::English] = "Invalid selection, please try again.";
    s_strings["pleaseSelect"][Language::English] = "Please select";
    s_strings["saveAsDefault"][Language::English] = "Save these options as default configuration?";
    s_strings["continueOrDone"][Language::English] = "Continue selecting or type 'done' to finish";

    // Additional common strings
    s_strings["yes"][Language::English] = "Yes";
    s_strings["no"][Language::English] = "No";
    s_strings["cancel"][Language::English] = "Cancel";
    s_strings["continue"][Language::English] = "Continue";
    s_strings["back"][Language::English] = "Back";
    s_strings["next"][Language::English] = "Next";
    s_strings["previous"][Language::English] = "Previous";
    s_strings["finish"][Language::English] = "Finish";
    s_strings["exit"][Language::English] = "Exit";
    s_strings["help"][Language::English] = "Help";
    s_strings["about"][Language::English] = "About";
    s_strings["version"][Language::English] = "Version";
    s_strings["options"][Language::English] = "Options";
    s_strings["settings"][Language::English] = "Settings";
    s_strings["configuration"][Language::English] = "Configuration";
    s_strings["advanced"][Language::English] = "Advanced";
    s_strings["basic"][Language::English] = "Basic";
    s_strings["custom"][Language::English] = "Custom";
    s_strings["default"][Language::English] = "Default";
    s_strings["recommended"][Language::English] = "Recommended";
    s_strings["optional"][Language::English] = "Optional";
    s_strings["required"][Language::English] = "Required";
    s_strings["enabled"][Language::English] = "Enabled";
    s_strings["disabled"][Language::English] = "Disabled";
    s_strings["warning"][Language::English] = "Warning";
    s_strings["error"][Language::English] = "Error";
    s_strings["info"][Language::English] = "Information";
    s_strings["success"][Language::English] = "Success";
}

// Load Chinese strings (using English placeholders for now)
void Localization::loadChineseStrings() {
    // Copy English strings as placeholders
    for (const auto& pair : s_strings) {
        if (pair.second.find(Language::English) != pair.second.end()) {
            s_strings[pair.first][Language::Chinese] = pair.second.at(Language::English);
        }
    }
}

// Load Spanish strings
void Localization::loadSpanishStrings() {
    s_strings["welcomeMessage"][Language::Spanish] =
            "¡Bienvenido a CPP-Scaffold! Vamos a crear tu proyecto C++.";
    s_strings["projectName"][Language::Spanish] = "Nombre del proyecto";
    s_strings["projectNameEmpty"][Language::Spanish] =
            "El nombre del proyecto no puede estar vacío.";
    s_strings["selectProjectType"][Language::Spanish] = "Seleccionar tipo de proyecto";
    s_strings["selectBuildSystem"][Language::Spanish] = "Seleccionar sistema de construcción";
    s_strings["selectPackageManager"][Language::Spanish] = "Seleccionar gestor de paquetes";
    s_strings["includeTests"][Language::Spanish] = "¿Incluir framework de pruebas unitarias?";
    s_strings["selectTestFramework"][Language::Spanish] = "Seleccionar framework de pruebas";
    s_strings["includeDocumentation"][Language::Spanish] =
            "¿Incluir configuración de documentación?";
    s_strings["includeCodeStyleTools"][Language::Spanish] =
            "¿Incluir herramientas de estilo de código y análisis estático?";
    s_strings["selectEditors"][Language::Spanish] = "Seleccionar editores a configurar";
    s_strings["selectCISystems"][Language::Spanish] = "Seleccionar sistemas CI/CD a configurar";
    s_strings["initGit"][Language::Spanish] = "¿Inicializar repositorio Git?";
    s_strings["invalidSelection"][Language::Spanish] =
            "Selección inválida, por favor intenta de nuevo.";
    s_strings["pleaseSelect"][Language::Spanish] = "Por favor selecciona";
    s_strings["saveAsDefault"][Language::Spanish] =
            "¿Guardar estas opciones como configuración por defecto?";
    s_strings["continueOrDone"][Language::Spanish] =
            "Continuar seleccionando o escribir 'done' para terminar";

    // Additional common strings
    s_strings["yes"][Language::Spanish] = "Sí";
    s_strings["no"][Language::Spanish] = "No";
    s_strings["cancel"][Language::Spanish] = "Cancelar";
    s_strings["continue"][Language::Spanish] = "Continuar";
    s_strings["back"][Language::Spanish] = "Atrás";
    s_strings["next"][Language::Spanish] = "Siguiente";
    s_strings["previous"][Language::Spanish] = "Anterior";
    s_strings["finish"][Language::Spanish] = "Terminar";
    s_strings["exit"][Language::Spanish] = "Salir";
    s_strings["help"][Language::Spanish] = "Ayuda";
    s_strings["about"][Language::Spanish] = "Acerca de";
    s_strings["version"][Language::Spanish] = "Versión";
    s_strings["options"][Language::Spanish] = "Opciones";
    s_strings["settings"][Language::Spanish] = "Configuración";
    s_strings["configuration"][Language::Spanish] = "Configuración";
    s_strings["advanced"][Language::Spanish] = "Avanzado";
    s_strings["basic"][Language::Spanish] = "Básico";
    s_strings["custom"][Language::Spanish] = "Personalizado";
    s_strings["default"][Language::Spanish] = "Por defecto";
    s_strings["recommended"][Language::Spanish] = "Recomendado";
    s_strings["optional"][Language::Spanish] = "Opcional";
    s_strings["required"][Language::Spanish] = "Requerido";
    s_strings["enabled"][Language::Spanish] = "Habilitado";
    s_strings["disabled"][Language::Spanish] = "Deshabilitado";
    s_strings["warning"][Language::Spanish] = "Advertencia";
    s_strings["error"][Language::Spanish] = "Error";
    s_strings["info"][Language::Spanish] = "Información";
    s_strings["success"][Language::Spanish] = "Éxito";
}

// Load Japanese strings (using English placeholders for now)
void Localization::loadJapaneseStrings() {
    // Copy English strings as placeholders
    for (const auto& pair : s_strings) {
        if (pair.second.find(Language::English) != pair.second.end()) {
            s_strings[pair.first][Language::Japanese] = pair.second.at(Language::English);
        }
    }
}

// Load German strings (using English placeholders for now)
void Localization::loadGermanStrings() {
    // Copy English strings as placeholders
    for (const auto& pair : s_strings) {
        if (pair.second.find(Language::English) != pair.second.end()) {
            s_strings[pair.first][Language::German] = pair.second.at(Language::English);
        }
    }
}

// Load French strings (using English placeholders for now)
void Localization::loadFrenchStrings() {
    // Copy English strings as placeholders
    for (const auto& pair : s_strings) {
        if (pair.second.find(Language::English) != pair.second.end()) {
            s_strings[pair.first][Language::French] = pair.second.at(Language::English);
        }
    }
}