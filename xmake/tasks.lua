-- Custom Tasks for cpp-scaffold
-- Defines all custom build tasks

-- Run Tests Task
task("run-tests")
    set_menu {
        usage = "xmake run-tests",
        description = "Run all unit tests",
        options = {
            {nil, "verbose", "v", nil, "Show verbose test output"},
            {nil, "filter", "f", "kv", "Filter tests by pattern"}
        }
    }
    on_run(function ()
        import("core.project.config")
        import("core.base.option")

        -- Build tests first
        os.exec("xmake build --group=Tests")

        local verbose = option.get("verbose")
        local filter = option.get("filter")

        -- Run all test executables
        local test_binaries = {
            "test_file_utils", "test_string_utils", "test_terminal_utils", "test_http_client",
            "test_cli_parser", "test_config_manager", "test_config_validator", "test_ci_config",
            "test_template_manager", "test_archive_template", "test_git_repository_template",
            "test_doc_generator", "test_test_framework_manager", "test_end_to_end",
            "test_integration", "test_msys2_integration", "test_cli_edge_cases",
            "test_template_edge_cases", "test_file_utils_edge_cases"
        }

        for _, test_name in ipairs(test_binaries) do
            local test_path = path.join("tests", "bin", test_name)
            if os.isfile(test_path) or os.isfile(test_path .. ".exe") then
                print("Running " .. test_name .. "...")
                local cmd = test_path
                if verbose then
                    cmd = cmd .. " --gtest_verbose"
                end
                if filter then
                    cmd = cmd .. " --gtest_filter=" .. filter
                end
                os.exec(cmd)
            end
        end
    end)
task_end()

-- Run Examples Task
task("run-examples")
    set_menu {
        usage = "xmake run-examples",
        description = "Build and run example projects",
        options = {
            {nil, "example", "e", "kv", "Run specific example"}
        }
    }
    on_run(function ()
        import("core.base.option")

        -- Build examples first
        os.exec("xmake build --group=Examples")

        local example = option.get("example")
        local examples = {
            "file-processor", "qt-image-viewer", "boost-network-server",
            "sfml-game-demo", "remote-source-demo"
        }

        if example then
            local example_path = path.join("examples", "bin", example)
            if os.isfile(example_path) or os.isfile(example_path .. ".exe") then
                print("Running " .. example .. "...")
                os.exec(example_path)
            else
                print("Example " .. example .. " not found or not built")
            end
        else
            print("Available examples:")
            for _, ex in ipairs(examples) do
                local example_path = path.join("examples", "bin", ex)
                if os.isfile(example_path) or os.isfile(example_path .. ".exe") then
                    print("  " .. ex .. " (built)")
                else
                    print("  " .. ex .. " (not built)")
                end
            end
        end
    end)
task_end()

-- Clean All Task
task("clean-all")
    set_menu {
        usage = "xmake clean-all",
        description = "Clean all build artifacts including tests and examples"
    }
    on_run(function ()
        -- Clean main build
        os.exec("xmake clean")

        -- Clean test and example directories
        if os.isdir("tests/bin") then
            os.rmdir("tests/bin")
        end
        if os.isdir("examples/bin") then
            os.rmdir("examples/bin")
        end

        print("All build artifacts cleaned")
    end)
task_end()

-- Coverage Task
task("coverage")
    set_menu {
        usage = "xmake coverage",
        description = "Generate code coverage report"
    }
    on_run(function ()
        -- Build with coverage
        os.exec("xmake config --coverage=true")
        os.exec("xmake build --group=Tests")

        -- Run tests
        os.exec("xmake run-tests")

        -- Generate coverage report
        if os.isfile("gcov") or os.isfile("llvm-cov") then
            print("Generating coverage report...")
            os.exec("gcov src/**/*.cpp")
            if os.isfile("lcov") then
                os.exec("lcov --capture --directory . --output-file coverage.info")
                os.exec("genhtml coverage.info --output-directory coverage_html")
                print("Coverage report generated in coverage_html/")
            end
        else
            print("Coverage tools not found. Install gcov/lcov for coverage reports.")
        end
    end)
task_end()

-- Format Task
task("format")
    set_menu {
        usage = "xmake format",
        description = "Format source code using clang-format"
    }
    on_run(function ()
        if os.isfile("clang-format") then
            print("Formatting source code...")
            os.exec("find src -name '*.cpp' -o -name '*.h' | xargs clang-format -i")
            os.exec("find tests -name '*.cpp' -o -name '*.h' | xargs clang-format -i")
            os.exec("find examples -name '*.cpp' -o -name '*.h' | xargs clang-format -i")
            print("Code formatting complete")
        else
            print("clang-format not found. Please install clang-format.")
        end
    end)
task_end()

-- Info Task
task("info")
    set_menu {
        usage = "xmake info",
        description = "Show build system information and available targets"
    }
    on_run(function ()
        print("CPP-Scaffold Xmake Build System")
        print("================================")
        print("")
        print("Available build modes:")
        print("  debug   - Debug build with symbols")
        print("  release - Optimized release build")
        print("")
        print("Available options:")
        print("  --tests    - Enable test building")
        print("  --examples - Enable example building")
        print("  --coverage - Enable code coverage")
        print("")
        print("Available tasks:")
        print("  run-tests    - Run all unit tests")
        print("  run-examples - Build and run examples")
        print("  clean-all    - Clean all artifacts")
        print("  coverage     - Generate coverage report")
        print("  format       - Format source code")
        print("  info         - Show this information")
        print("")
        print("Example usage:")
        print("  xmake config --tests=true --examples=true")
        print("  xmake build")
        print("  xmake run-tests")
        print("  xmake run-examples")
        print("")
        print("Target groups:")
        print("  Components - Core library components")
        print("  Executables - Main application")
        print("  Tests - Unit and integration tests")
        print("  Examples - Example projects")
    end)
task_end()

-- MSYS2-specific tasks
local msystem = os.getenv("MSYSTEM")
local mingw_prefix = os.getenv("MINGW_PREFIX")
local msys2_detected = msystem ~= nil or mingw_prefix ~= nil

if msys2_detected then
    -- MSYS2 Package Installation Task
    task("msys2-install-deps")
        set_menu {
            usage = "xmake msys2-install-deps",
            description = "Install MSYS2 dependencies using pacman"
        }
        on_run(function ()
            -- MSYS2 package mapping
            local msys2_packages = {
                "mingw-w64-spdlog",
                "mingw-w64-nlohmann-json",
                "mingw-w64-gtest",
                "mingw-w64-boost",
                "mingw-w64-qt5-base",
                "mingw-w64-sfml",
                "mingw-w64-cmake",
                "mingw-w64-ninja",
                "mingw-w64-make"
            }

            print("Installing MSYS2 packages...")
            local cmd = "pacman -S --needed --noconfirm " .. table.concat(msys2_packages, " ")
            local result = os.exec(cmd)

            if result == 0 then
                print("MSYS2 dependencies installed successfully")
            else
                print("Failed to install MSYS2 dependencies")
            end
        end)
    task_end()

    -- MSYS2 Environment Info Task
    task("msys2-info")
        set_menu {
            usage = "xmake msys2-info",
            description = "Show MSYS2 environment information"
        }
        on_run(function ()
            print("MSYS2 Environment Information")
            print("============================")
            print("Environment: " .. (msystem or "Not detected"))
            print("MinGW Prefix: " .. (mingw_prefix or "Not set"))
            print("MSYS2 Root: " .. (os.getenv("MSYS2_ROOT") or "Not set"))
            print("")
            print("Architecture Support:")
            print("  MinGW64: " .. (msystem == "MINGW64" and "Yes" or "No"))
            print("  MinGW32: " .. (msystem == "MINGW32" and "Yes" or "No"))
            print("  UCRT64:  " .. (msystem == "UCRT64" and "Yes" or "No"))
            print("  Clang64: " .. (msystem == "CLANG64" and "Yes" or "No"))
            print("  ClangARM64: " .. (msystem == "CLANGARM64" and "Yes" or "No"))
            print("")

            -- Check for essential tools
            local tools = {"gcc", "g++", "make", "cmake", "ninja", "pacman"}
            print("Available Tools:")
            for _, tool in ipairs(tools) do
                local available = os.isexec(tool) and "Yes" or "No"
                print("  " .. tool .. ": " .. available)
            end
        end)
    task_end()

    -- MSYS2 Package Creation Task
    task("msys2-package")
        set_menu {
            usage = "xmake msys2-package",
            description = "Create MSYS2 package (PKGBUILD)"
        }
        on_run(function ()
            print("Creating MSYS2 package...")

            -- Create PKGBUILD file
            local pkgbuild_content = [[
# Maintainer: Your Name <your.email@example.com>
pkgname=mingw-w64-cpp-scaffold
pkgver=1.0.0
pkgrel=1
pkgdesc="C++ project scaffolding tool (mingw-w64)"
arch=('any')
mingw_arch=('mingw32' 'mingw64' 'ucrt64' 'clang64' 'clangarm64')
url="https://github.com/yourname/cpp-scaffold"
license=('MIT')
depends=("${MINGW_PACKAGE_PREFIX}-gcc-libs"
         "${MINGW_PACKAGE_PREFIX}-spdlog"
         "${MINGW_PACKAGE_PREFIX}-nlohmann-json")
makedepends=("${MINGW_PACKAGE_PREFIX}-cc"
             "${MINGW_PACKAGE_PREFIX}-cmake"
             "${MINGW_PACKAGE_PREFIX}-ninja"
             "${MINGW_PACKAGE_PREFIX}-xmake")
source=("cpp-scaffold-${pkgver}.tar.gz")
sha256sums=('SKIP')

build() {
    cd "${srcdir}/cpp-scaffold-${pkgver}"

    # Configure with xmake
    xmake config --mode=release --msys2_package=true

    # Build the project
    xmake build
}

package() {
    cd "${srcdir}/cpp-scaffold-${pkgver}"

    # Install the binary
    install -Dm755 "bin/cpp-scaffold.exe" "${pkgdir}${MINGW_PREFIX}/bin/cpp-scaffold.exe"

    # Install documentation
    install -Dm644 README.md "${pkgdir}${MINGW_PREFIX}/share/doc/${pkgname}/README.md"
    install -Dm644 LICENSE "${pkgdir}${MINGW_PREFIX}/share/licenses/${pkgname}/LICENSE"
}
]]

            -- Write PKGBUILD file
            io.writefile("PKGBUILD", pkgbuild_content)
            print("PKGBUILD created successfully")
            print("To build the package, run: makepkg -s")
        end)
    task_end()

    -- MSYS2 Development Environment Setup
    task("msys2-setup-dev")
        set_menu {
            usage = "xmake msys2-setup-dev",
            description = "Setup MSYS2 development environment"
        }
        on_run(function ()
            print("Setting up MSYS2 development environment...")

            -- Essential development packages
            local dev_packages = {
                "mingw-w64-toolchain",
                "mingw-w64-cmake",
                "mingw-w64-ninja",
                "mingw-w64-make",
                "mingw-w64-gdb",
                "mingw-w64-spdlog",
                "mingw-w64-nlohmann-json",
                "mingw-w64-gtest",
                "git"
            }

            print("Installing development packages...")
            local cmd = "pacman -S --needed --noconfirm " .. table.concat(dev_packages, " ")
            os.exec(cmd)

            print("MSYS2 development environment setup complete!")
            print("You can now build the project with: xmake build")
        end)
    task_end()
end
