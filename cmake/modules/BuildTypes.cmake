# BuildTypes.cmake
# Enhanced build type configuration with better compiler flags and optimization settings

# Define custom build types
function(define_custom_build_types)
    # Get the list of available configurations
    get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    
    if(isMultiConfig)
        # Multi-config generators (Visual Studio, Xcode)
        set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo;FastDebug;Profile" CACHE STRING "" FORCE)
    else()
        # Single-config generators (Ninja, Unix Makefiles)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS 
            "Debug" "Release" "MinSizeRel" "RelWithDebInfo" "FastDebug" "Profile")
    endif()
endfunction()

# Configure Debug build type
function(configure_debug_build_type)
    # Debug flags for different compilers
    if(MSVC)
        set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /RTC1 /MDd" CACHE STRING "Debug CXX flags" FORCE)
        set(CMAKE_C_FLAGS_DEBUG "/Od /Zi /RTC1 /MDd" CACHE STRING "Debug C flags" FORCE)
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/DEBUG /INCREMENTAL" CACHE STRING "Debug linker flags" FORCE)
    else()
        set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fno-omit-frame-pointer -fstack-protector-strong" CACHE STRING "Debug CXX flags" FORCE)
        set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -fno-omit-frame-pointer -fstack-protector-strong" CACHE STRING "Debug C flags" FORCE)
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "" CACHE STRING "Debug linker flags" FORCE)
    endif()
endfunction()

# Configure Release build type
function(configure_release_build_type)
    # Release flags for different compilers
    if(MSVC)
        set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /DNDEBUG /MD" CACHE STRING "Release CXX flags" FORCE)
        set(CMAKE_C_FLAGS_RELEASE "/O2 /Ob2 /DNDEBUG /MD" CACHE STRING "Release C flags" FORCE)
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO" CACHE STRING "Release linker flags" FORCE)
    else()
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -ffunction-sections -fdata-sections" CACHE STRING "Release CXX flags" FORCE)
        set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -ffunction-sections -fdata-sections" CACHE STRING "Release C flags" FORCE)
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections" CACHE STRING "Release linker flags" FORCE)
    endif()
endfunction()

# Configure MinSizeRel build type
function(configure_minsizerel_build_type)
    # MinSizeRel flags for different compilers
    if(MSVC)
        set(CMAKE_CXX_FLAGS_MINSIZEREL "/O1 /Ob1 /DNDEBUG /MD" CACHE STRING "MinSizeRel CXX flags" FORCE)
        set(CMAKE_C_FLAGS_MINSIZEREL "/O1 /Ob1 /DNDEBUG /MD" CACHE STRING "MinSizeRel C flags" FORCE)
        set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/INCREMENTAL:NO" CACHE STRING "MinSizeRel linker flags" FORCE)
    else()
        set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -ffunction-sections -fdata-sections" CACHE STRING "MinSizeRel CXX flags" FORCE)
        set(CMAKE_C_FLAGS_MINSIZEREL "-Os -DNDEBUG -ffunction-sections -fdata-sections" CACHE STRING "MinSizeRel C flags" FORCE)
        set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "-Wl,--gc-sections -s" CACHE STRING "MinSizeRel linker flags" FORCE)
    endif()
endfunction()

# Configure RelWithDebInfo build type
function(configure_relwithdebinfo_build_type)
    # RelWithDebInfo flags for different compilers
    if(MSVC)
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /Ob1 /Zi /DNDEBUG /MD" CACHE STRING "RelWithDebInfo CXX flags" FORCE)
        set(CMAKE_C_FLAGS_RELWITHDEBINFO "/O2 /Ob1 /Zi /DNDEBUG /MD" CACHE STRING "RelWithDebInfo C flags" FORCE)
        set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/DEBUG /INCREMENTAL:NO" CACHE STRING "RelWithDebInfo linker flags" FORCE)
    else()
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG -ffunction-sections -fdata-sections" CACHE STRING "RelWithDebInfo CXX flags" FORCE)
        set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG -ffunction-sections -fdata-sections" CACHE STRING "RelWithDebInfo C flags" FORCE)
        set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "-Wl,--gc-sections" CACHE STRING "RelWithDebInfo linker flags" FORCE)
    endif()
endfunction()

# Configure FastDebug build type (optimized debug)
function(configure_fastdebug_build_type)
    # FastDebug flags for different compilers
    if(MSVC)
        set(CMAKE_CXX_FLAGS_FASTDEBUG "/Og /Zi /MDd" CACHE STRING "FastDebug CXX flags" FORCE)
        set(CMAKE_C_FLAGS_FASTDEBUG "/Og /Zi /MDd" CACHE STRING "FastDebug C flags" FORCE)
        set(CMAKE_EXE_LINKER_FLAGS_FASTDEBUG "/DEBUG /INCREMENTAL" CACHE STRING "FastDebug linker flags" FORCE)
    else()
        set(CMAKE_CXX_FLAGS_FASTDEBUG "-Og -g -fno-omit-frame-pointer" CACHE STRING "FastDebug CXX flags" FORCE)
        set(CMAKE_C_FLAGS_FASTDEBUG "-Og -g -fno-omit-frame-pointer" CACHE STRING "FastDebug C flags" FORCE)
        set(CMAKE_EXE_LINKER_FLAGS_FASTDEBUG "" CACHE STRING "FastDebug linker flags" FORCE)
    endif()
endfunction()

# Configure Profile build type (for profiling)
function(configure_profile_build_type)
    # Profile flags for different compilers
    if(MSVC)
        set(CMAKE_CXX_FLAGS_PROFILE "/O2 /Zi /DNDEBUG /MD" CACHE STRING "Profile CXX flags" FORCE)
        set(CMAKE_C_FLAGS_PROFILE "/O2 /Zi /DNDEBUG /MD" CACHE STRING "Profile C flags" FORCE)
        set(CMAKE_EXE_LINKER_FLAGS_PROFILE "/DEBUG /PROFILE" CACHE STRING "Profile linker flags" FORCE)
    else()
        set(CMAKE_CXX_FLAGS_PROFILE "-O2 -g -pg -DNDEBUG" CACHE STRING "Profile CXX flags" FORCE)
        set(CMAKE_C_FLAGS_PROFILE "-O2 -g -pg -DNDEBUG" CACHE STRING "Profile C flags" FORCE)
        set(CMAKE_EXE_LINKER_FLAGS_PROFILE "-pg" CACHE STRING "Profile linker flags" FORCE)
    endif()
endfunction()

# Apply build type specific configurations to a target
function(apply_build_type_config target)
    # Apply configurations based on current build type
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${target} PRIVATE DEBUG _DEBUG)
        if(ENABLE_SANITIZERS)
            if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
                target_compile_options(${target} PRIVATE -fsanitize=address -fsanitize=undefined)
                target_link_options(${target} PRIVATE -fsanitize=address -fsanitize=undefined)
            endif()
        endif()
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_definitions(${target} PRIVATE NDEBUG RELEASE)
        # Apply LTO if enabled
        if(ENABLE_LTO)
            set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
        endif()
    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        target_compile_definitions(${target} PRIVATE NDEBUG MINSIZEREL)
        if(ENABLE_LTO)
            set_target_properties(${target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
        endif()
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        target_compile_definitions(${target} PRIVATE NDEBUG RELWITHDEBINFO)
    elseif(CMAKE_BUILD_TYPE STREQUAL "FastDebug")
        target_compile_definitions(${target} PRIVATE DEBUG _DEBUG FASTDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Profile")
        target_compile_definitions(${target} PRIVATE NDEBUG PROFILE)
    endif()
endfunction()

# Setup all build types
function(setup_enhanced_build_types)
    define_custom_build_types()
    configure_debug_build_type()
    configure_release_build_type()
    configure_minsizerel_build_type()
    configure_relwithdebinfo_build_type()
    configure_fastdebug_build_type()
    configure_profile_build_type()
    
    # Set default build type if not specified
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
    endif()
    
    message(STATUS "Enhanced build types configured")
    message(STATUS "Available build types: Debug, Release, MinSizeRel, RelWithDebInfo, FastDebug, Profile")
    message(STATUS "Current build type: ${CMAKE_BUILD_TYPE}")
endfunction()

# Print build type information
function(print_build_type_info)
    message(STATUS "")
    message(STATUS "=== Build Type Configuration ===")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
    
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Debug build: Full debugging information, no optimizations")
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "Release build: Full optimizations, no debugging information")
    elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        message(STATUS "MinSizeRel build: Size optimizations, no debugging information")
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        message(STATUS "RelWithDebInfo build: Optimizations with debugging information")
    elseif(CMAKE_BUILD_TYPE STREQUAL "FastDebug")
        message(STATUS "FastDebug build: Light optimizations with debugging information")
    elseif(CMAKE_BUILD_TYPE STREQUAL "Profile")
        message(STATUS "Profile build: Optimizations with profiling support")
    endif()
    
    message(STATUS "================================")
    message(STATUS "")
endfunction()
