# 完整的构建系统和模板类型组合测试脚本

$ErrorActionPreference = "Stop"

# 定义测试组合
$buildSystems = @("cmake", "xmake", "premake", "meson", "bazel")
$templateTypes = @("console", "lib", "gui", "header-only-lib", "modules", "webservice", "gameengine")

# 测试结果统计
$totalTests = 0
$passedTests = 0
$failedTests = 0
$failedCombinations = @()

Write-Host "🧪 开始完整的构建系统和模板类型组合测试..." -ForegroundColor Green
Write-Host "测试组合数量: $($buildSystems.Count * $templateTypes.Count)" -ForegroundColor Yellow

foreach ($buildSystem in $buildSystems) {
    foreach ($templateType in $templateTypes) {
        $totalTests++
        $projectName = "test-$buildSystem-$templateType"
        
        Write-Host "`n[$totalTests] 测试: $buildSystem + $templateType" -ForegroundColor Cyan
        
        try {
            # 清理可能存在的项目目录
            if (Test-Path $projectName) {
                Remove-Item -Recurse -Force $projectName
            }
            
            # 创建项目
            $output = & "./build/bin/cpp-scaffold.exe" create $projectName --template $templateType --build $buildSystem --tests gtest 2>&1
            
            if ($LASTEXITCODE -eq 0) {
                # 验证项目结构
                $projectExists = Test-Path $projectName
                
                # 验证构建文件
                $buildFileExists = $false
                switch ($buildSystem) {
                    "cmake" { $buildFileExists = Test-Path "$projectName/CMakeLists.txt" }
                    "xmake" { $buildFileExists = Test-Path "$projectName/xmake.lua" }
                    "premake" { $buildFileExists = Test-Path "$projectName/premake5.lua" }
                    "meson" { $buildFileExists = Test-Path "$projectName/meson.build" }
                    "bazel" { $buildFileExists = Test-Path "$projectName/BUILD" }
                }
                
                if ($projectExists -and $buildFileExists) {
                    Write-Host "  ✅ 通过" -ForegroundColor Green
                    $passedTests++
                } else {
                    Write-Host "  ❌ 失败: 项目结构不完整" -ForegroundColor Red
                    $failedTests++
                    $failedCombinations += "$buildSystem + $templateType (结构不完整)"
                }
            } else {
                Write-Host "  ❌ 失败: 项目创建失败" -ForegroundColor Red
                Write-Host "  错误输出: $output" -ForegroundColor Red
                $failedTests++
                $failedCombinations += "$buildSystem + $templateType (创建失败)"
            }
            
            # 清理测试项目
            if (Test-Path $projectName) {
                Remove-Item -Recurse -Force $projectName
            }
            
        } catch {
            Write-Host "  ❌ 异常: $($_.Exception.Message)" -ForegroundColor Red
            $failedTests++
            $failedCombinations += "$buildSystem + $templateType (异常)"
        }
    }
}

# 输出测试结果
Write-Host "`n" + "="*60 -ForegroundColor Yellow
Write-Host "🧪 测试完成!" -ForegroundColor Green
Write-Host "总测试数: $totalTests" -ForegroundColor White
Write-Host "通过: $passedTests" -ForegroundColor Green
Write-Host "失败: $failedTests" -ForegroundColor Red
Write-Host "成功率: $([math]::Round(($passedTests / $totalTests) * 100, 2))%" -ForegroundColor Yellow

if ($failedTests -gt 0) {
    Write-Host "`n❌ 失败的组合:" -ForegroundColor Red
    foreach ($failed in $failedCombinations) {
        Write-Host "  - $failed" -ForegroundColor Red
    }
    exit 1
} else {
    Write-Host "`n🎉 所有测试都通过了!" -ForegroundColor Green
    exit 0
}
