@echo off
setlocal enabledelayedexpansion

echo Building cpp-scaffold project...

:: Change to build directory
cd build

:: Detect number of CPU cores for parallel compilation
for /f "tokens=2 delims==" %%i in ('wmic cpu get NumberOfLogicalProcessors /value ^| find "="') do set CORES=%%i

:: Use all available cores, but cap at 8 to avoid overwhelming the system
if %CORES% gtr 8 set CORES=8

echo Using %CORES% parallel jobs for compilation...

:: Build with parallel jobs
ninja -j%CORES%

if %ERRORLEVEL% neq 0 (
    echo Build failed with error code %ERRORLEVEL%
    exit /b %ERRORLEVEL%
) else (
    echo Build completed successfully!
)
