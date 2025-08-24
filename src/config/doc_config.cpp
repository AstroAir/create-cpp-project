#include "doc_config.h"
#include "../utils/file_utils.h"

#include <spdlog/spdlog.h>

using namespace utils;

bool DocConfig::createDocConfig(const std::string &projectPath,
                                const CliOptions &options) {

  // Create docs directory
  std::string docsDir = FileUtils::combinePath(projectPath, "docs");
  if (!FileUtils::createDirectory(docsDir)) {
    spdlog::error("Failed to create docs directory");
    return false;
  }

  // Create Doxygen configuration
  if (!createDoxygenConfig(projectPath, options)) {
    spdlog::error("Failed to create Doxygen configuration");
    return false;
  }

  // Create documentation generation script
  std::string scriptName = "generate_docs";

#ifdef _WIN32
  scriptName += ".bat";
#else
  scriptName += ".sh";
#endif

  std::string scriptPath = FileUtils::combinePath(docsDir, scriptName);
  if (!FileUtils::writeToFile(scriptPath, getDocGenScriptContent(options))) {
    spdlog::error("Failed to create documentation generation script");
    return false;
  }

  // Make script executable on Unix-like systems
#ifndef _WIN32
  system(("chmod +x \"" + scriptPath + "\"").c_str());
#endif

  spdlog::info("Documentation configuration created successfully");
  return true;
}

bool DocConfig::createDoxygenConfig(const std::string &projectPath,
                                    const CliOptions &options) {
  std::string doxyfilePath =
      FileUtils::combinePath(projectPath, "docs/Doxyfile");
  return FileUtils::writeToFile(doxyfilePath, getDoxyfileContent(options));
}

bool DocConfig::createSphinxConfig(const std::string &projectPath,
                                   const CliOptions &options) {
  // This method would create Sphinx configuration if needed
  // Currently not implemented
  (void)projectPath; // TODO: Implement Sphinx configuration
  (void)options;     // TODO: Use options for Sphinx configuration
  return true;
}

std::string DocConfig::getDoxyfileContent(const CliOptions &options) {
  return R"(# Doxyfile for )" + options.projectName + R"(

# Project related configuration options
PROJECT_NAME           = ")" +
         options.projectName + R"("
PROJECT_NUMBER         = "0.1.0"
PROJECT_BRIEF          = "A C++ Project"
OUTPUT_DIRECTORY       = docs/generated
CREATE_SUBDIRS         = NO
ALLOW_UNICODE_NAMES    = YES
OUTPUT_LANGUAGE        = English
OUTPUT_TEXT_DIRECTION  = None
BRIEF_MEMBER_DESC      = YES
REPEAT_BRIEF           = YES
ABBREVIATE_BRIEF       = "The $name class" \
                         "The $name widget" \
                         "The $name file" \
                         is \
                         provides \
                         specifies \
                         contains \
                         represents \
                         a \
                         an \
                         the
ALWAYS_DETAILED_SEC    = NO
INLINE_INHERITED_MEMB  = NO
FULL_PATH_NAMES        = YES
STRIP_FROM_PATH        =
STRIP_FROM_INC_PATH    =
SHORT_NAMES            = NO
JAVADOC_AUTOBRIEF      = YES
JAVADOC_BANNER         = NO
QT_AUTOBRIEF           = NO
MULTILINE_CPP_IS_BRIEF = NO
PYTHON_DOCSTRING       = YES
INHERIT_DOCS           = YES
SEPARATE_MEMBER_PAGES  = NO
TAB_SIZE               = 4
ALIASES                =
OPTIMIZE_OUTPUT_FOR_C  = NO
OPTIMIZE_OUTPUT_JAVA   = NO
OPTIMIZE_FOR_FORTRAN   = NO
OPTIMIZE_OUTPUT_VHDL   = NO
OPTIMIZE_OUTPUT_SLICE  = NO
EXTENSION_MAPPING      =
MARKDOWN_SUPPORT       = YES
TOC_INCLUDE_HEADINGS   = 5
AUTOLINK_SUPPORT       = YES
BUILTIN_STL_SUPPORT    = YES
CPP_CLI_SUPPORT        = NO
SIP_SUPPORT            = NO
IDL_PROPERTY_SUPPORT   = YES
DISTRIBUTE_GROUP_DOC   = NO
GROUP_NESTED_COMPOUNDS = NO
SUBGROUPING            = YES
INLINE_GROUPED_CLASSES = NO
INLINE_SIMPLE_STRUCTS  = NO
TYPEDEF_HIDES_STRUCT   = NO
LOOKUP_CACHE_SIZE      = 0
NUM_PROC_THREADS       = 1

# Build related options
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = YES
EXTRACT_PRIV_VIRTUAL   = YES
EXTRACT_PACKAGE        = NO
EXTRACT_STATIC         = YES
EXTRACT_LOCAL_CLASSES  = YES
EXTRACT_LOCAL_METHODS  = YES
EXTRACT_ANON_NSPACES   = YES
RESOLVE_UNNAMED_PARAMS = YES
HIDE_UNDOC_MEMBERS     = NO
HIDE_UNDOC_CLASSES     = NO
HIDE_FRIEND_COMPOUNDS  = NO
HIDE_IN_BODY_DOCS      = NO
INTERNAL_DOCS          = YES
CASE_SENSE_NAMES       = YES
HIDE_SCOPE_NAMES       = NO
HIDE_COMPOUND_REFERENCE= NO
SHOW_INCLUDE_FILES     = YES
SHOW_GROUPED_MEMB_INC  = NO
FORCE_LOCAL_INCLUDES   = NO
INLINE_INFO            = YES
SORT_MEMBER_DOCS       = YES
SORT_BRIEF_DOCS        = NO
SORT_MEMBERS_CTORS_1ST = NO
SORT_GROUP_NAMES       = NO
SORT_BY_SCOPE_NAME     = NO
STRICT_PROTO_MATCHING  = NO
GENERATE_TODOLIST      = YES
GENERATE_TESTLIST      = YES
GENERATE_BUGLIST       = YES
GENERATE_DEPRECATEDLIST= YES
ENABLED_SECTIONS       =
MAX_INITIALIZER_LINES  = 30
SHOW_USED_FILES        = YES
SHOW_FILES             = YES
SHOW_NAMESPACES        = YES
FILE_VERSION_FILTER    =
LAYOUT_FILE            =
CITE_BIB_FILES         =

# Input and output configuration
INPUT                  = include/ src/
INPUT_ENCODING         = UTF-8
FILE_PATTERNS          = *.c \
                         *.cc \
                         *.cxx \
                         *.cpp \
                         *.c++ \
                         *.h \
                         *.hh \
                         *.hxx \
                         *.hpp \
                         *.h++ \
                         *.md
RECURSIVE              = YES
EXCLUDE                =
EXCLUDE_SYMLINKS       = NO
EXCLUDE_PATTERNS       = */build/* \
                         */tests/* \
                         */third_party/*
EXCLUDE_SYMBOLS        =
EXAMPLE_PATH           =
EXAMPLE_PATTERNS       = *
EXAMPLE_RECURSIVE      = NO
IMAGE_PATH             =
INPUT_FILTER           =
FILTER_PATTERNS        =
FILTER_SOURCE_FILES    = NO
FILTER_SOURCE_PATTERNS =
USE_MDFILE_AS_MAINPAGE = README.md
SOURCE_BROWSER         = YES
INLINE_SOURCES         = NO
STRIP_CODE_COMMENTS    = YES
REFERENCED_BY_RELATION = YES
REFERENCES_RELATION    = YES
REFERENCES_LINK_SOURCE = YES
SOURCE_TOOLTIPS        = YES
USE_HTAGS              = NO
VERBATIM_HEADERS       = YES
ALPHABETICAL_INDEX     = YES
IGNORE_PREFIX          =

# HTML output configuration
GENERATE_HTML          = YES
HTML_OUTPUT            = html
HTML_FILE_EXTENSION    = .html
HTML_HEADER            =
HTML_FOOTER            =
HTML_STYLESHEET        =
HTML_EXTRA_STYLESHEET  =
HTML_EXTRA_FILES       =
HTML_COLORSTYLE_HUE    = 220
HTML_COLORSTYLE_SAT    = 100
HTML_COLORSTYLE_GAMMA  = 80
HTML_TIMESTAMP         = YES
HTML_DYNAMIC_MENUS     = YES
HTML_DYNAMIC_SECTIONS  = NO
HTML_INDEX_NUM_ENTRIES = 100
GENERATE_DOCSET        = NO
GENERATE_HTMLHELP      = NO
GENERATE_QHP           = NO
GENERATE_ECLIPSEHELP   = NO
DISABLE_INDEX          = NO
GENERATE_TREEVIEW      = YES
ENUM_VALUES_PER_LINE   = 4
TREEVIEW_WIDTH         = 250
EXT_LINKS_IN_WINDOW    = NO
HTML_FORMULA_FORMAT    = png
FORMULA_FONTSIZE       = 10
FORMULA_TRANSPARENT    = YES
SEARCHENGINE           = YES

# Other formats
GENERATE_LATEX         = NO
GENERATE_RTF           = NO
GENERATE_MAN           = NO
GENERATE_XML           = NO
GENERATE_DOCBOOK       = NO
GENERATE_AUTOGEN_DEF   = NO
GENERATE_PERLMOD       = NO

# Preprocessor
ENABLE_PREPROCESSING   = YES
MACRO_EXPANSION        = YES
EXPAND_ONLY_PREDEF     = NO
SEARCH_INCLUDES        = YES
INCLUDE_PATH           =
INCLUDE_FILE_PATTERNS  =
PREDEFINED             =
EXPAND_AS_DEFINED      =
SKIP_FUNCTION_MACROS   = YES

# Dot Diagrams
HAVE_DOT               = YES
DOT_NUM_THREADS        = 0
CLASS_GRAPH            = YES
COLLABORATION_GRAPH    = YES
GROUP_GRAPHS           = YES
UML_LOOK               = NO
UML_LIMIT_NUM_FIELDS   = 10
DOT_UML_DETAILS        = NO
DOT_WRAP_THRESHOLD     = 17
TEMPLATE_RELATIONS     = YES
INCLUDE_GRAPH          = YES
INCLUDED_BY_GRAPH      = YES
CALL_GRAPH             = YES
CALLER_GRAPH           = YES
GRAPHICAL_HIERARCHY    = YES
DIRECTORY_GRAPH        = YES
DIR_GRAPH_MAX_DEPTH    = 1
DOT_IMAGE_FORMAT       = svg
INTERACTIVE_SVG        = YES
DOT_PATH               =
DOTFILE_DIRS           =
MSCFILE_DIRS           =
DIAFILE_DIRS           =
PLANTUML_JAR_PATH      =
PLANTUML_CFG_FILE      =
PLANTUML_INCLUDE_PATH  =
DOT_GRAPH_MAX_NODES    = 50
MAX_DOT_GRAPH_DEPTH    = 0
DOT_TRANSPARENT        = NO
DOT_MULTI_TARGETS      = NO
GENERATE_LEGEND        = YES
DOT_CLEANUP            = YES
)";
}

std::string DocConfig::getDocGenScriptContent(const CliOptions &options) {
#ifdef _WIN32
  return R"(@echo off
REM Documentation generation script for )" +
         options.projectName + R"(

REM Check if Doxygen is installed
where doxygen >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Doxygen is not installed. Please install it to generate documentation.
    echo Download from: https://www.doxygen.nl/download.html
    exit /b 1
)

echo Generating documentation with Doxygen...
cd %~dp0..
doxygen docs/Doxyfile

if %ERRORLEVEL% neq 0 (
    echo Error generating documentation!
    exit /b 1
)

echo Documentation generated successfully.
echo Open docs/generated/html/index.html in your browser to view it.
)";
#else
  return R"(#!/bin/bash
# Documentation generation script for )" +
         options.projectName + R"(

# Change to project root directory
cd "$(dirname "$0")/.."

# Check if Doxygen is installed
if ! command -v doxygen &> /dev/null; then
    echo "Doxygen is not installed. Please install it to generate documentation."
    echo "On Ubuntu/Debian: sudo apt-get install doxygen"
    echo "On macOS with Homebrew: brew install doxygen"
    exit 1
fi

echo "Generating documentation with Doxygen..."
doxygen docs/Doxyfile

if [ $? -ne 0 ]; then
    echo "Error generating documentation!"
    exit 1
fi

echo "Documentation generated successfully."
echo "Open docs/generated/html/index.html in your browser to view it."
)";
#endif
}