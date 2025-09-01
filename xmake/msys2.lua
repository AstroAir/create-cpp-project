-- MSYS2 Environment Support for cpp-scaffold
-- Provides MSYS2-specific configurations, package management, and optimizations

local msys2 = {}

-- MSYS2 Environment Detection
function msys2.detect_environment()
    local msystem = os.getenv("MSYSTEM")
    local mingw_prefix = os.getenv("MINGW_PREFIX")
    local msys2_root = os.getenv("MSYS2_ROOT")
    
    return {
        detected = msystem ~= nil or mingw_prefix ~= nil,
        msystem = msystem,
        mingw_prefix = mingw_prefix,
        msys2_root = msys2_root,
        is_mingw64 = msystem == "MINGW64",
        is_mingw32 = msystem == "MINGW32",
        is_ucrt64 = msystem == "UCRT64",
        is_clang64 = msystem == "CLANG64",
        is_clangarm64 = msystem == "CLANGARM64"
    }
end

-- MSYS2 Package Name Mapping
msys2.package_mapping = {
    -- Core libraries
    ["spdlog"] = "mingw-w64-spdlog",
    ["nlohmann_json"] = "mingw-w64-nlohmann-json",
    ["gtest"] = "mingw-w64-gtest",
    ["boost"] = "mingw-w64-boost",
    
    -- GUI frameworks
    ["qt5widgets"] = "mingw-w64-qt5-base",
    ["qt6widgets"] = "mingw-w64-qt6-base",
    ["sfml"] = "mingw-w64-sfml",
    ["gtk3"] = "mingw-w64-gtk3",
    ["wxwidgets"] = "mingw-w64-wxwidgets",
    
    -- Development tools
    ["cmake"] = "mingw-w64-cmake",
    ["ninja"] = "mingw-w64-ninja",
    ["make"] = "mingw-w64-make",
    ["gdb"] = "mingw-w64-gdb",
    ["clang"] = "mingw-w64-clang",
    ["gcc"] = "mingw-w64-gcc",
    
    -- Compression and networking
    ["zlib"] = "mingw-w64-zlib",
    ["openssl"] = "mingw-w64-openssl",
    ["curl"] = "mingw-w64-curl",
    ["libarchive"] = "mingw-w64-libarchive"
}

-- Apply MSYS2-specific compiler configurations
function msys2.apply_compiler_config()
    local env = msys2.detect_environment()
    if not env.detected then
        return
    end
    
    print("Configuring for MSYS2 environment: " .. (env.msystem or "unknown"))
    
    -- Add MSYS2-specific definitions
    add_defines("MSYS2_BUILD=1", "MINGW_BUILD=1")
    if env.msystem then
        add_defines("MSYSTEM=\"" .. env.msystem .. "\"")
    end
    
    -- MSYS2-specific compiler flags
    if env.mingw_prefix then
        add_includedirs(path.join(env.mingw_prefix, "include"), {system = true})
        add_linkdirs(path.join(env.mingw_prefix, "lib"))
    end
    
    -- Windows-specific libraries for MSYS2
    if is_plat("windows") then
        add_syslinks("ws2_32", "shlwapi", "advapi32", "shell32", "user32", "kernel32")
    end
    
    -- MSYS2-specific optimization flags
    if is_mode("release") then
        if env.is_mingw64 or env.is_mingw32 then
            add_cxflags("-march=native", "-mtune=native")
            add_cxflags("-ffast-math", "-funroll-loops")
            -- Link-time optimization
            add_cxflags("-flto")
            add_ldflags("-flto")
        elseif env.is_clang64 or env.is_clangarm64 then
            add_cxflags("-march=native", "-mtune=native")
            add_cxflags("-ffast-math")
        end
    end
    
    -- MSYS2-specific runtime configuration
    if env.is_ucrt64 then
        -- Use UCRT runtime
        add_cxflags("-D_UCRT")
    end
end

-- Configure MSYS2 package requirements
function msys2.configure_packages()
    local env = msys2.detect_environment()
    if not env.detected then
        return
    end
    
    -- Override package names for MSYS2
    for xmake_name, msys2_name in pairs(msys2.package_mapping) do
        -- Check if package is already required
        if has_package(xmake_name) then
            print("Using MSYS2 package: " .. msys2_name .. " for " .. xmake_name)
        end
    end
end

-- MSYS2-specific build optimizations
function msys2.apply_build_optimizations()
    local env = msys2.detect_environment()
    if not env.detected then
        return
    end
    
    -- Parallel build optimization
    if env.mingw_prefix then
        -- Use all available CPU cores for parallel builds
        local cpu_count = os.cpuinfo().ncpu or 4
        set_policy("build.jobs", cpu_count)
    end
    
    -- MSYS2-specific linker optimizations
    if is_mode("release") then
        add_ldflags("-Wl,--gc-sections")  -- Remove unused sections
        add_ldflags("-Wl,--strip-all")    -- Strip symbols
        
        if env.is_mingw64 or env.is_mingw32 then
            add_ldflags("-Wl,--enable-auto-import")
            add_ldflags("-Wl,--enable-runtime-pseudo-reloc")
        end
    end
end

-- Create MSYS2 package installation helper
function msys2.install_packages(packages)
    local env = msys2.detect_environment()
    if not env.detected then
        print("MSYS2 environment not detected. Please run from MSYS2 shell.")
        return false
    end
    
    local msys2_packages = {}
    for _, pkg in ipairs(packages) do
        local msys2_pkg = msys2.package_mapping[pkg] or pkg
        table.insert(msys2_packages, msys2_pkg)
    end
    
    if #msys2_packages > 0 then
        local cmd = "pacman -S --needed --noconfirm " .. table.concat(msys2_packages, " ")
        print("Installing MSYS2 packages: " .. table.concat(msys2_packages, ", "))
        os.exec(cmd)
    end
    
    return true
end

-- MSYS2 environment validation
function msys2.validate_environment()
    local env = msys2.detect_environment()
    
    if not env.detected then
        print("Warning: MSYS2 environment not detected")
        print("Please ensure you're running from an MSYS2 shell (MinGW64, MinGW32, UCRT64, etc.)")
        return false
    end
    
    -- Check for essential tools
    local essential_tools = {"gcc", "g++", "make", "pacman"}
    for _, tool in ipairs(essential_tools) do
        if not os.isexec(tool) then
            print("Warning: " .. tool .. " not found in PATH")
            print("Please install mingw-w64-toolchain: pacman -S mingw-w64-x86_64-toolchain")
        end
    end
    
    print("MSYS2 environment validated successfully")
    print("Environment: " .. env.msystem)
    if env.mingw_prefix then
        print("MinGW Prefix: " .. env.mingw_prefix)
    end
    
    return true
end

-- MSYS2-specific target configuration
function msys2.configure_target(target_name)
    local env = msys2.detect_environment()
    if not env.detected then
        return
    end
    
    target(target_name)
        -- Add MSYS2-specific configurations
        if env.mingw_prefix then
            add_includedirs(path.join(env.mingw_prefix, "include"), {system = true})
            add_linkdirs(path.join(env.mingw_prefix, "lib"))
        end
        
        -- MSYS2-specific runtime libraries
        if is_kind("binary") then
            add_syslinks("ws2_32", "shlwapi", "advapi32")
        end
        
        -- Set proper Windows subsystem for GUI applications
        if target_name:find("gui") or target_name:find("qt") or target_name:find("sfml") then
            if is_mode("release") then
                add_ldflags("-mwindows")  -- Windows subsystem (no console)
            end
        end
    target_end()
end

-- Export functions
return {
    detect_environment = msys2.detect_environment,
    apply_compiler_config = msys2.apply_compiler_config,
    configure_packages = msys2.configure_packages,
    apply_build_optimizations = msys2.apply_build_optimizations,
    install_packages = msys2.install_packages,
    validate_environment = msys2.validate_environment,
    configure_target = msys2.configure_target,
    package_mapping = msys2.package_mapping
}
