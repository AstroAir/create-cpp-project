-- Build Options for cpp-scaffold
-- Defines build options and their application logic

-- Detect MSYS2 environment
local msystem = os.getenv("MSYSTEM")
local mingw_prefix = os.getenv("MINGW_PREFIX")
local msys2_detected = msystem ~= nil or mingw_prefix ~= nil

-- Build Options
option("tests")
    set_default(false)
    set_showmenu(true)
    set_description("Enable building tests")
option_end()

option("examples")
    set_default(false)
    set_showmenu(true)
    set_description("Enable building examples")
option_end()

option("coverage")
    set_default(false)
    set_showmenu(true)
    set_description("Enable code coverage")
option_end()

-- MSYS2-specific options
if msys2_detected then
    option("msys2_optimizations")
        set_default(true)
        set_showmenu(true)
        set_description("Enable MSYS2-specific optimizations")
    option_end()

    option("msys2_static")
        set_default(false)
        set_showmenu(true)
        set_description("Build with static linking for MSYS2")
    option_end()

    option("msys2_package")
        set_default(false)
        set_showmenu(true)
        set_description("Prepare build for MSYS2 packaging")
    option_end()
end


