#include "localization.h"

// Initialize static members
Language Localization::s_currentLanguage = Language::English;
std::unordered_map<std::string, std::unordered_map<Language, std::string>>
    Localization::s_strings;

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
  s_strings["projectNameEmpty"][Language::English] =
      "Project name cannot be empty.";
  s_strings["selectProjectType"][Language::English] = "Select project type";
  s_strings["selectBuildSystem"][Language::English] = "Select build system";
  s_strings["selectPackageManager"][Language::English] =
      "Select package manager";
  s_strings["includeTests"][Language::English] = "Include unit test framework?";
  s_strings["selectTestFramework"][Language::English] = "Select test framework";
  s_strings["includeDocumentation"][Language::English] =
      "Include documentation configuration?";
  s_strings["includeCodeStyleTools"][Language::English] =
      "Include code style and static analysis tools?";
  s_strings["selectEditors"][Language::English] = "Select editors to configure";
  s_strings["selectCISystems"][Language::English] =
      "Select CI/CD systems to configure";
  s_strings["initGit"][Language::English] = "Initialize Git repository?";
  s_strings["invalidSelection"][Language::English] =
      "Invalid selection, please try again.";
  s_strings["pleaseSelect"][Language::English] = "Please select";
  s_strings["saveAsDefault"][Language::English] =
      "Save these options as default?";
  s_strings["continueOrDone"][Language::English] =
      "Continue selecting or type 'done' to finish";

  // Additional common strings
  s_strings["yes"][Language::English] = "Yes";
  s_strings["no"][Language::English] = "No";
  s_strings["cancel"][Language::English] = "Cancel";
  s_strings["continue"][Language::English] = "Continue";
  s_strings["back"][Language::English] = "Back";
  s_strings["next"][Language::English] = "Next";
  s_strings["finish"][Language::English] = "Finish";
  s_strings["error"][Language::English] = "Error";
  s_strings["warning"][Language::English] = "Warning";
  s_strings["info"][Language::English] = "Information";
  s_strings["success"][Language::English] = "Success";
}

// Load Chinese strings
void Localization::loadChineseStrings() {
  s_strings["welcomeMessage"][Language::Chinese] =
      "欢迎使用 CPP-Scaffold! 让我们创建您的 C++ 项目。";
  s_strings["projectName"][Language::Chinese] = "项目名称";
  s_strings["projectNameEmpty"][Language::Chinese] = "项目名称不能为空。";
  s_strings["selectProjectType"][Language::Chinese] = "选择项目类型";
  s_strings["selectBuildSystem"][Language::Chinese] = "选择构建系统";
  s_strings["selectPackageManager"][Language::Chinese] = "选择包管理器";
  s_strings["includeTests"][Language::Chinese] = "包含单元测试框架？";
  s_strings["selectTestFramework"][Language::Chinese] = "选择测试框架";
  s_strings["includeDocumentation"][Language::Chinese] =
      "是否包含项目文档配置?";
  s_strings["includeCodeStyleTools"][Language::Chinese] =
      "是否包含代码风格和静态分析工具?";
  s_strings["selectEditors"][Language::Chinese] = "选择要配置的编辑器支持";
  s_strings["selectCISystems"][Language::Chinese] = "选择要配置的CI/CD系统";
  s_strings["initGit"][Language::Chinese] = "初始化 Git 仓库？";
  s_strings["invalidSelection"][Language::Chinese] = "选择无效，请重试。";
  s_strings["pleaseSelect"][Language::Chinese] = "请选择";
  s_strings["saveAsDefault"][Language::Chinese] =
      "是否保存这些选项作为默认配置?";
  s_strings["continueOrDone"][Language::Chinese] = "继续选择或输入 'done' 完成";

  // Additional common strings
  s_strings["yes"][Language::Chinese] = "是";
  s_strings["no"][Language::Chinese] = "否";
  s_strings["cancel"][Language::Chinese] = "取消";
  s_strings["continue"][Language::Chinese] = "继续";
  s_strings["back"][Language::Chinese] = "返回";
  s_strings["next"][Language::Chinese] = "下一步";
  s_strings["finish"][Language::Chinese] = "完成";
  s_strings["error"][Language::Chinese] = "错误";
  s_strings["warning"][Language::Chinese] = "警告";
  s_strings["info"][Language::Chinese] = "信息";
  s_strings["success"][Language::Chinese] = "成功";
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
  s_strings["selectCISystems"][Language::Spanish] =
      "Seleccionar sistemas CI/CD a configurar";
  s_strings["initGit"][Language::Spanish] = "¿Inicializar repositorio Git?";
  s_strings["invalidSelection"][Language::Spanish] =
      "Selección inválida, por favor intenta de nuevo.";
  s_strings["pleaseSelect"][Language::Spanish] = "Por favor selecciona";
  s_strings["saveAsDefault"][Language::Spanish] =
      "¿Guardar estas opciones como predeterminadas?";
  s_strings["continueOrDone"][Language::Spanish] =
      "Continuar seleccionando o escribir 'done' para terminar";

  // Additional common strings
  s_strings["yes"][Language::Spanish] = "Sí";
  s_strings["no"][Language::Spanish] = "No";
  s_strings["cancel"][Language::Spanish] = "Cancelar";
  s_strings["continue"][Language::Spanish] = "Continuar";
  s_strings["back"][Language::Spanish] = "Atrás";
  s_strings["next"][Language::Spanish] = "Siguiente";
  s_strings["finish"][Language::Spanish] = "Finalizar";
  s_strings["error"][Language::Spanish] = "Error";
  s_strings["warning"][Language::Spanish] = "Advertencia";
  s_strings["info"][Language::Spanish] = "Información";
  s_strings["success"][Language::Spanish] = "Éxito";
}

// Load Japanese strings
void Localization::loadJapaneseStrings() {
  s_strings["welcomeMessage"][Language::Japanese] =
      "CPP-Scaffoldへようこそ！C++プロジェクトを作成しましょう。";
  s_strings["projectName"][Language::Japanese] = "プロジェクト名";
  s_strings["projectNameEmpty"][Language::Japanese] =
      "プロジェクト名を空にすることはできません。";
  s_strings["selectProjectType"][Language::Japanese] = "プロジェクトタイプを選択";
  s_strings["selectBuildSystem"][Language::Japanese] = "ビルドシステムを選択";
  s_strings["selectPackageManager"][Language::Japanese] = "パッケージマネージャーを選択";
  s_strings["includeTests"][Language::Japanese] = "単体テストフレームワークを含めますか？";
  s_strings["selectTestFramework"][Language::Japanese] = "テストフレームワークを選択";
  s_strings["includeDocumentation"][Language::Japanese] =
      "ドキュメント設定を含めますか？";
  s_strings["includeCodeStyleTools"][Language::Japanese] =
      "コードスタイルと静的解析ツールを含めますか？";
  s_strings["selectEditors"][Language::Japanese] = "設定するエディターを選択";
  s_strings["selectCISystems"][Language::Japanese] =
      "設定するCI/CDシステムを選択";
  s_strings["initGit"][Language::Japanese] = "Gitリポジトリを初期化しますか？";
  s_strings["invalidSelection"][Language::Japanese] =
      "無効な選択です。もう一度お試しください。";
  s_strings["pleaseSelect"][Language::Japanese] = "選択してください";
  s_strings["saveAsDefault"][Language::Japanese] =
      "これらのオプションをデフォルトとして保存しますか？";
  s_strings["continueOrDone"][Language::Japanese] =
      "選択を続けるか、'done'と入力して終了してください";

  // Additional common strings
  s_strings["yes"][Language::Japanese] = "はい";
  s_strings["no"][Language::Japanese] = "いいえ";
  s_strings["cancel"][Language::Japanese] = "キャンセル";
  s_strings["continue"][Language::Japanese] = "続行";
  s_strings["back"][Language::Japanese] = "戻る";
  s_strings["next"][Language::Japanese] = "次へ";
  s_strings["finish"][Language::Japanese] = "完了";
  s_strings["error"][Language::Japanese] = "エラー";
  s_strings["warning"][Language::Japanese] = "警告";
  s_strings["info"][Language::Japanese] = "情報";
  s_strings["success"][Language::Japanese] = "成功";
}

// Load German strings
void Localization::loadGermanStrings() {
  s_strings["welcomeMessage"][Language::German] =
      "Willkommen bei CPP-Scaffold! Lassen Sie uns Ihr C++ Projekt erstellen.";
  s_strings["projectName"][Language::German] = "Projektname";
  s_strings["projectNameEmpty"][Language::German] =
      "Projektname darf nicht leer sein.";
  s_strings["selectProjectType"][Language::German] = "Projekttyp auswählen";
  s_strings["selectBuildSystem"][Language::German] = "Build-System auswählen";
  s_strings["selectPackageManager"][Language::German] = "Paketmanager auswählen";
  s_strings["includeTests"][Language::German] = "Unit-Test-Framework einschließen?";
  s_strings["selectTestFramework"][Language::German] = "Test-Framework auswählen";
  s_strings["includeDocumentation"][Language::German] =
      "Dokumentationskonfiguration einschließen?";
  s_strings["includeCodeStyleTools"][Language::German] =
      "Code-Style- und statische Analysetools einschließen?";
  s_strings["selectEditors"][Language::German] = "Zu konfigurierende Editoren auswählen";
  s_strings["selectCISystems"][Language::German] =
      "Zu konfigurierende CI/CD-Systeme auswählen";
  s_strings["initGit"][Language::German] = "Git-Repository initialisieren?";
  s_strings["invalidSelection"][Language::German] =
      "Ungültige Auswahl, bitte versuchen Sie es erneut.";
  s_strings["pleaseSelect"][Language::German] = "Bitte auswählen";
  s_strings["saveAsDefault"][Language::German] =
      "Diese Optionen als Standard speichern?";
  s_strings["continueOrDone"][Language::German] =
      "Weiter auswählen oder 'done' eingeben zum Beenden";

  // Additional common strings
  s_strings["yes"][Language::German] = "Ja";
  s_strings["no"][Language::German] = "Nein";
  s_strings["cancel"][Language::German] = "Abbrechen";
  s_strings["continue"][Language::German] = "Fortfahren";
  s_strings["back"][Language::German] = "Zurück";
  s_strings["next"][Language::German] = "Weiter";
  s_strings["finish"][Language::German] = "Fertig";
  s_strings["error"][Language::German] = "Fehler";
  s_strings["warning"][Language::German] = "Warnung";
  s_strings["info"][Language::German] = "Information";
  s_strings["success"][Language::German] = "Erfolg";
}

// Load French strings
void Localization::loadFrenchStrings() {
  s_strings["welcomeMessage"][Language::French] =
      "Bienvenue dans CPP-Scaffold ! Créons votre projet C++.";
  s_strings["projectName"][Language::French] = "Nom du projet";
  s_strings["projectNameEmpty"][Language::French] =
      "Le nom du projet ne peut pas être vide.";
  s_strings["selectProjectType"][Language::French] = "Sélectionner le type de projet";
  s_strings["selectBuildSystem"][Language::French] = "Sélectionner le système de build";
  s_strings["selectPackageManager"][Language::French] = "Sélectionner le gestionnaire de paquets";
  s_strings["includeTests"][Language::French] = "Inclure un framework de tests unitaires ?";
  s_strings["selectTestFramework"][Language::French] = "Sélectionner le framework de test";
  s_strings["includeDocumentation"][Language::French] =
      "Inclure la configuration de documentation ?";
  s_strings["includeCodeStyleTools"][Language::French] =
      "Inclure les outils de style de code et d'analyse statique ?";
  s_strings["selectEditors"][Language::French] = "Sélectionner les éditeurs à configurer";
  s_strings["selectCISystems"][Language::French] =
      "Sélectionner les systèmes CI/CD à configurer";
  s_strings["initGit"][Language::French] = "Initialiser le dépôt Git ?";
  s_strings["invalidSelection"][Language::French] =
      "Sélection invalide, veuillez réessayer.";
  s_strings["pleaseSelect"][Language::French] = "Veuillez sélectionner";
  s_strings["saveAsDefault"][Language::French] =
      "Sauvegarder ces options par défaut ?";
  s_strings["continueOrDone"][Language::French] =
      "Continuer la sélection ou taper 'done' pour terminer";

  // Additional common strings
  s_strings["yes"][Language::French] = "Oui";
  s_strings["no"][Language::French] = "Non";
  s_strings["cancel"][Language::French] = "Annuler";
  s_strings["continue"][Language::French] = "Continuer";
  s_strings["back"][Language::French] = "Retour";
  s_strings["next"][Language::French] = "Suivant";
  s_strings["finish"][Language::French] = "Terminer";
  s_strings["error"][Language::French] = "Erreur";
  s_strings["warning"][Language::French] = "Avertissement";
  s_strings["info"][Language::French] = "Information";
  s_strings["success"][Language::French] = "Succès";
}
