#include "code_style_config.h"
#include "../utils/file_utils.h"

#include <spdlog/spdlog.h>

using namespace utils;

bool CodeStyleConfig::createCodeStyleConfig(const std::string &projectPath,
                                            const CliOptions &options) {

  bool success = true;

  // Create clang-format config
  if (!createClangFormatConfig(projectPath)) {
    spdlog::error("Failed to create clang-format configuration");
    success = false;
  } else {
    spdlog::info("clang-format configuration created successfully");
  }

  // Create clang-tidy config
  if (!createClangTidyConfig(projectPath)) {
    spdlog::error("Failed to create clang-tidy configuration");
    success = false;
  } else {
    spdlog::info("clang-tidy configuration created successfully");
  }

  // Create EditorConfig
  if (!createEditorConfig(projectPath)) {
    spdlog::error("Failed to create EditorConfig configuration");
    success = false;
  } else {
    spdlog::info("EditorConfig configuration created successfully");
  }

  return success;
}

bool CodeStyleConfig::createClangFormatConfig(const std::string &projectPath) {
  std::string formatPath = FileUtils::combinePath(projectPath, ".clang-format");
  return FileUtils::writeToFile(formatPath, getClangFormatContent());
}

bool CodeStyleConfig::createClangTidyConfig(const std::string &projectPath) {
  std::string tidyPath = FileUtils::combinePath(projectPath, ".clang-tidy");
  return FileUtils::writeToFile(tidyPath, getClangTidyContent());
}

bool CodeStyleConfig::createEditorConfig(const std::string &projectPath) {
  std::string editorConfigPath =
      FileUtils::combinePath(projectPath, ".editorconfig");
  return FileUtils::writeToFile(editorConfigPath, getEditorConfigContent());
}

std::string CodeStyleConfig::getClangFormatContent() {
  return R"(# C++ format style configuration
Language: Cpp
BasedOnStyle: Google
AccessModifierOffset: -2
AlignAfterOpenBracket: Align
AlignConsecutiveAssignments: false
AlignConsecutiveDeclarations: false
AlignEscapedNewlines: Left
AlignOperands: true
AlignTrailingComments: true
AllowAllParametersOfDeclarationOnNextLine: true
AllowShortBlocksOnASingleLine: false
AllowShortCaseLabelsOnASingleLine: false
AllowShortFunctionsOnASingleLine: All
AllowShortIfStatementsOnASingleLine: false
AllowShortLoopsOnASingleLine: false
AlwaysBreakAfterDefinitionReturnType: None
AlwaysBreakAfterReturnType: None
AlwaysBreakBeforeMultilineStrings: true
AlwaysBreakTemplateDeclarations: Yes
BinPackArguments: true
BinPackParameters: true
BraceWrapping:
  AfterClass: false
  AfterControlStatement: false
  AfterEnum: false
  AfterFunction: false
  AfterNamespace: false
  AfterObjCDeclaration: false
  AfterStruct: false
  AfterUnion: false
  AfterExternBlock: false
  BeforeCatch: false
  BeforeElse: false
  IndentBraces: false
  SplitEmptyFunction: true
  SplitEmptyRecord: true
  SplitEmptyNamespace: true
BreakBeforeBinaryOperators: None
BreakBeforeBraces: Attach
BreakBeforeInheritanceComma: false
BreakInheritanceList: BeforeColon
BreakBeforeTernaryOperators: true
BreakConstructorInitializersBeforeComma: false
BreakConstructorInitializers: BeforeColon
BreakAfterJavaFieldAnnotations: false
BreakStringLiterals: true
ColumnLimit: 100
CommentPragmas: '^ IWYU pragma:'
CompactNamespaces: false
ConstructorInitializerAllOnOneLineOrOnePerLine: true
ConstructorInitializerIndentWidth: 4
ContinuationIndentWidth: 4
Cpp11BracedListStyle: true
DerivePointerAlignment: true
DisableFormat: false
ExperimentalAutoDetectBinPacking: false
FixNamespaceComments: true
ForEachMacros:
  - foreach
  - Q_FOREACH
  - BOOST_FOREACH
IncludeBlocks: Regroup
IncludeCategories:
  - Regex: '^<ext/.*\.h>'
    Priority: 2
  - Regex: '^<.*\.h>'
    Priority: 1
  - Regex: '^<.*'
    Priority: 2
  - Regex: '.*'
    Priority: 3
IncludeIsMainRegex: '([-_](test|unittest))?$'
IndentCaseLabels: true
IndentPPDirectives: None
IndentWidth: 2
IndentWrappedFunctionNames: false
JavaScriptQuotes: Leave
JavaScriptWrapImports: true
KeepEmptyLinesAtTheStartOfBlocks: false
MacroBlockBegin: ''
MacroBlockEnd: ''
MaxEmptyLinesToKeep: 1
NamespaceIndentation: None
ObjCBinPackProtocolList: Never
ObjCBlockIndentWidth: 2
ObjCSpaceAfterProperty: false
ObjCSpaceBeforeProtocolList: true
PenaltyBreakAssignment: 2
PenaltyBreakBeforeFirstCallParameter: 1
PenaltyBreakComment: 300
PenaltyBreakFirstLessLess: 120
PenaltyBreakString: 1000
PenaltyBreakTemplateDeclaration: 10
PenaltyExcessCharacter: 1000000
PenaltyReturnTypeOnItsOwnLine: 200
PointerAlignment: Left
RawStringFormats:
  - Language: Cpp
    Delimiters:
      - cc
      - CC
      - cpp
      - Cpp
      - CPP
      - 'c++'
      - 'C++'
    CanonicalDelimiter: ''
    BasedOnStyle: google
  - Language: TextProto
    Delimiters:
      - pb
      - PB
      - proto
      - PROTO
    EnclosingFunctions:
      - EqualsProto
      - EquivToProto
      - PARSE_PARTIAL_TEXT_PROTO
      - PARSE_TEST_PROTO
      - PARSE_TEXT_PROTO
      - ParseTextOrDie
      - ParseTextProtoOrDie
    CanonicalDelimiter: ''
    BasedOnStyle: google
ReflowComments: true
SortIncludes: true
SortUsingDeclarations: true
SpaceAfterCStyleCast: false
SpaceAfterTemplateKeyword: true
SpaceBeforeAssignmentOperators: true
SpaceBeforeCpp11BracedList: false
SpaceBeforeCtorInitializerColon: true
SpaceBeforeInheritanceColon: true
SpaceBeforeParens: ControlStatements
SpaceBeforeRangeBasedForLoopColon: true
SpaceInEmptyParentheses: false
SpacesBeforeTrailingComments: 2
SpacesInAngles: false
SpacesInContainerLiterals: true
SpacesInCStyleCastParentheses: false
SpacesInParentheses: false
SpacesInSquareBrackets: false
Standard: Auto
StatementMacros:
  - Q_UNUSED
  - QT_REQUIRE_VERSION
TabWidth: 8
UseTab: Never
)";
}

std::string CodeStyleConfig::getClangTidyContent() {
  return R"(---
Checks: 'clang-diagnostic-*,clang-analyzer-*,cppcoreguidelines-*,modernize-*,bugprone-*,performance-*,readability-*,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-cppcoreguidelines-pro-type-union-access,-cppcoreguidelines-pro-bounds-pointer-arithmetic'
WarningsAsErrors: ''
HeaderFilterRegex: ''
AnalyzeTemporaryDtors: false
FormatStyle: none
CheckOptions:
  - key: readability-identifier-naming.ClassCase
    value: CamelCase
  - key: readability-identifier-naming.PrivateMemberSuffix
    value: _
  - key: readability-identifier-naming.ConstexprVariableCase
    value: UPPER_CASE
  - key: modernize-use-nullptr.NullMacros
    value: 'NULL'
  - key: readability-identifier-naming.EnumConstantCase
    value: UPPER_CASE
  - key: readability-identifier-naming.FunctionCase
    value: camelBack
  - key: readability-identifier-naming.MemberCase
    value: camelBack
  - key: readability-identifier-naming.ParameterCase
    value: camelBack
  - key: readability-identifier-naming.UnionCase
    value: CamelCase
  - key: readability-identifier-naming.VariableCase
    value: camelBack
  - key: readability-identifier-naming.ConstantCase
    value: UPPER_CASE
...
)";
}

std::string CodeStyleConfig::getEditorConfigContent() {
  return R"(# EditorConfig helps maintain consistent coding styles across various editors and IDEs
# https://editorconfig.org/

root = true

[*]
charset = utf-8
end_of_line = lf
indent_size = 2
indent_style = space
insert_final_newline = true
max_line_length = 100
trim_trailing_whitespace = true

[*.{cpp,h,hpp,cc,c,cxx}]
indent_size = 2
indent_style = space

[{CMakeLists.txt,*.cmake}]
indent_size = 2
indent_style = space

[*.md]
max_line_length = off
trim_trailing_whitespace = false

[*.json]
indent_size = 2
indent_style = space

[*.yml]
indent_size = 2
indent_style = space

[Makefile]
indent_style = tab
)";
}