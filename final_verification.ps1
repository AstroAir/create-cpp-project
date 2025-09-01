# 最终验证测试 - 测试关键的构建系统组合

$ErrorActionPreference = "Stop"

Write-Host "🔍 最终验证测试开始..." -ForegroundColor Green

# 测试关键组合
$tests = @(
    @{name="XMake Console"; build="xmake"; template="console"},
    @{name="Premake Library"; build="premake"; template="lib"},
    @{name="Meson GUI"; build="meson"; template="gui"},
    @{name="XMake Modules"; build="xmake"; template="modules"},
    @{name="Premake GameEngine"; build="premake"; template="gameengine"},
    @{name="XMake WebService"; build="xmake"; template="webservice"}
)

$passed = 0
$failed = 0

foreach ($test in $tests) {
    $projectName = "verify-$($test.build)-$($test.template)"
    
    Write-Host "`n测试: $($test.name)" -ForegroundColor Cyan
    
    try {
        # 清理
        if (Test-Path $projectName) {
            Remove-Item -Recurse -Force $projectName
        }
        
        # 创建项目
        $output = & "./build/bin/cpp-scaffold.exe" create $projectName --template $test.template --build $test.build --tests gtest 2>&1
        
        if ($LASTEXITCODE -eq 0) {
            # 验证构建文件
            $buildFile = switch ($test.build) {
                "xmake" { "$projectName/xmake.lua" }
                "premake" { "$projectName/premake5.lua" }
                "meson" { "$projectName/meson.build" }
                "cmake" { "$projectName/CMakeLists.txt" }
                "bazel" { "$projectName/BUILD" }
            }
            
            if (Test-Path $buildFile) {
                Write-Host "  ✅ 通过 - 构建文件存在: $buildFile" -ForegroundColor Green
                $passed++
            } else {
                Write-Host "  ❌ 失败 - 构建文件缺失: $buildFile" -ForegroundColor Red
                $failed++
            }
        } else {
            Write-Host "  ❌ 失败 - 项目创建失败" -ForegroundColor Red
            $failed++
        }
        
        # 清理
        if (Test-Path $projectName) {
            Remove-Item -Recurse -Force $projectName
        }
        
    } catch {
        Write-Host "  ❌ 异常: $($_.Exception.Message)" -ForegroundColor Red
        $failed++
    }
}

# 结果
Write-Host "`n" + "="*50 -ForegroundColor Yellow
Write-Host "🔍 最终验证完成!" -ForegroundColor Green
Write-Host "通过: $passed" -ForegroundColor Green
Write-Host "失败: $failed" -ForegroundColor Red

if ($failed -eq 0) {
    Write-Host "`n🎉 所有关键功能都正常工作!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "`n❌ 发现 $failed 个问题需要修复" -ForegroundColor Red
    exit 1
}
