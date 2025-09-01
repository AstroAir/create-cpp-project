project('test-meson-project', 'cpp',
  version : '0.1.0',
  default_options : ['warning_level=3', 'cpp_std=c++17']
)

# Dependencies
spdlog_dep = dependency('spdlog')

# Qt dependencies
qt5_dep = dependency('qt5', modules : ['Core', 'Gui', 'Widgets'])
qt_deps = [qt5_dep]

# Test dependencies
doctest_dep = dependency('doctest')
test_deps = [doctest_dep]

# Source files
src_files = [
  'src/main.cpp',
  'src/application.cpp',
  'src/main_window.cpp',
  'src/logging.cpp',
]

# Include directories
inc_dirs = include_directories('include')

# Dependencies list
dependencies = [spdlog_dep, qt_deps]

# Main executable
executable('test-meson-project',
  src_files,
  include_directories : inc_dirs,
  dependencies : dependencies,
  install : true
)

# Tests
test_exe = executable('test_runner',
  ['tests/test_main.cpp'],
  include_directories : inc_dirs,
  dependencies : dependencies + test_deps
)

test('test-meson-project_tests', test_exe)
