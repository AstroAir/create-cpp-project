# 快速测试重要的构建系统组合

$ErrorActionPreference = "Stop"

# 定义重要的测试组合
$testCombinations = @(
    @{build="xmake"; template="console"; name="xmake-console"},
    @{build="xmake"; template="lib"; name="xmake-lib"},
    @{build="premake"; template="console"; name="premake-console"},
    @{build="premake"; template="lib"; name="premake-lib"},
    @{build="meson"; template="gui"; name="meson-gui"},
    @{build="xmake"; template="header-only-lib"; name="xmake-header"},
    @{build="premake"; template="gameengine"; name="premake-game"},
    @{build="xmake"; template="webservice"; name="xmake-web"}
)

$passed = 0
$failed = 0
$failedTests = @()

Write-Host "🧪 开始快速测试重要的构建系统组合..." -ForegroundColor Green

foreach ($test in $testCombinations) {
    $projectName = "quick-test-$($test.name)"
    
    Write-Host "`n测试: $($test.build) + $($test.template)" -ForegroundColor Cyan
    
    try {
        # 清理可能存在的项目目录
        if (Test-Path $projectName) {
            Remove-Item -Recurse -Force $projectName
        }
        
        # 创建项目
        $output = & "./build/bin/cpp-scaffold.exe" create $projectName --template $test.template --build $test.build --tests gtest 2>&1
        
        if ($LASTEXITCODE -eq 0) {
            # 验证构建文件
            $buildFileExists = $false
            switch ($test.build) {
                "cmake" { $buildFileExists = Test-Path "$projectName/CMakeLists.txt" }
                "xmake" { $buildFileExists = Test-Path "$projectName/xmake.lua" }
                "premake" { $buildFileExists = Test-Path "$projectName/premake5.lua" }
                "meson" { $buildFileExists = Test-Path "$projectName/meson.build" }
                "bazel" { $buildFileExists = Test-Path "$projectName/BUILD" }
            }
            
            if ($buildFileExists) {
                Write-Host "  ✅ 通过" -ForegroundColor Green
                $passed++
            } else {
                Write-Host "  ❌ 失败: 构建文件缺失" -ForegroundColor Red
                $failed++
                $failedTests += "$($test.build) + $($test.template)"
            }
        } else {
            Write-Host "  ❌ 失败: 项目创建失败" -ForegroundColor Red
            $failed++
            $failedTests += "$($test.build) + $($test.template)"
        }
        
        # 清理测试项目
        if (Test-Path $projectName) {
            Remove-Item -Recurse -Force $projectName
        }
        
    } catch {
        Write-Host "  ❌ 异常: $($_.Exception.Message)" -ForegroundColor Red
        $failed++
        $failedTests += "$($test.build) + $($test.template)"
    }
}

# 输出结果
Write-Host "`n" + "="*50 -ForegroundColor Yellow
Write-Host "🧪 快速测试完成!" -ForegroundColor Green
Write-Host "通过: $passed" -ForegroundColor Green
Write-Host "失败: $failed" -ForegroundColor Red

if ($failed -gt 0) {
    Write-Host "`n❌ 失败的测试:" -ForegroundColor Red
    foreach ($failedTest in $failedTests) {
        Write-Host "  - $failedTest" -ForegroundColor Red
    }
} else {
    Write-Host "`n🎉 所有重要组合都通过了!" -ForegroundColor Green
}
