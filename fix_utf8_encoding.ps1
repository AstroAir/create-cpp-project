#!/usr/bin/env pwsh

# UTF-8 Encoding Fix Script for C++ Project
# This script fixes corrupted Unicode characters in source files

Write-Host "Starting UTF-8 encoding fix..." -ForegroundColor Green

# Define the mapping of corrupted characters to correct UTF-8 characters
$utf8Fixes = @{
    # Progress bar characters
    "█" = "█"
    "░" = "░"
    "▓" = "▓"
    "▒" = "▒"
    
    # Status indicators
    "✓" = "✓"
    "✗" = "✗"
    "●" = "●"
    "○" = "○"
    "►" = "►"
    "☑" = "☑"
    "☐" = "☐"
    
    # Spinner characters (Braille patterns)
    "⠋" = "⠋"
    "⠙" = "⠙"
    "⠹" = "⠹"
    "⠸" = "⠸"
    "⠼" = "⠼"
    "⠴" = "⠴"
    "⠦" = "⠦"
    "⠧" = "⠧"
    "⠇" = "⠇"
    "⠏" = "⠏"
    
    # Box drawing characters
    "┌" = "┌"
    "┐" = "┐"
    "└" = "└"
    "┘" = "┘"
    "├" = "├"
    "┤" = "┤"
    "┬" = "┬"
    "┴" = "┴"
    "┼" = "┼"
    "─" = "─"
    "│" = "│"
    "╔" = "╔"
    "╗" = "╗"
    "╚" = "╚"
    "╝" = "╝"
    "╠" = "╠"
    "╣" = "╣"
    "╦" = "╦"
    "╩" = "╩"
    "╬" = "╬"
    "═" = "═"
    "║" = "║"
    "╭" = "╭"
    "╮" = "╮"
    "╯" = "╯"
    "╰" = "╰"
    "┏" = "┏"
    "┓" = "┓"
    "┗" = "┗"
    "┛" = "┛"
    "┣" = "┣"
    "┫" = "┫"
    "┳" = "┳"
    "┻" = "┻"
    "╋" = "╋"
    "━" = "━"
    "┃" = "┃"
    
    # Chinese characters
    "你好" = "你好"
    "世界" = "世界"
    "欢迎" = "欢迎"
    "使用" = "使用"
    "项目" = "项目"
    "脚手架" = "脚手架"
    "名称" = "名称"
    "不能" = "不能"
    "为空" = "为空"
    "包含" = "包含"
    "单元" = "单元"
    "测试" = "测试"
    "框架" = "框架"
    "选择" = "选择"
    "要配置" = "要配置"
    "编辑器" = "编辑器"
    "支持" = "支持"
    "初始化" = "初始化"
    "仓库" = "仓库"
    "无效" = "无效"
    "请重试" = "请重试"
    "是" = "是"
    "否" = "否"
    "下一步" = "下一步"
    
    # Japanese characters
    "こんにちは" = "こんにちは"
    "プロジェクト" = "プロジェクト"
    "名前" = "名前"
    "空にする" = "空にする"
    "ことはできません" = "ことはできません"
    "単体テスト" = "単体テスト"
    "フレームワーク" = "フレームワーク"
    "含めますか" = "含めますか"
    "選択" = "選択"
    "ドキュメント" = "ドキュメント"
    "設定" = "設定"
    "システム" = "システム"
    "無効な選択" = "無効な選択"
    "もう一度" = "もう一度"
    "お試しください" = "お試しください"
    "これらの" = "これらの"
    "オプション" = "オプション"
    "デフォルト" = "デフォルト"
    "として" = "として"
    "保存" = "保存"
    "しますか" = "しますか"
    "続ける" = "続ける"
    "done" = "done"
    "入力" = "入力"
    "して" = "して"
    "終了" = "終了"
    "してください" = "してください"
    "いいえ" = "いいえ"
    "キャンセル" = "キャンセル"
    "エラー" = "エラー"
    
    # Emojis and symbols
    "🚀" = "🚀"
    "🌟" = "🌟"
    "💻" = "💻"
    "🔧" = "🔧"
    "🎯" = "🎯"
    "📦" = "📦"
    "🛠️" = "🛠️"
    "🌈" = "🌈"
    "🎉" = "🎉"
    "⚠️" = "⚠️"
    "ℹ️" = "ℹ️"
    
    # Common corrupted patterns
    "鈻?" = "█"
    "鈼?" = "●"
    "鈼?" = "○"
    "鉁?" = "✓"
    "鉁?" = "✗"
    "鉅?" = "⠋"
    "鈹?" = "┌"
    "鈺?" = "╔"
    "馃彈锔?" = "⚠️"
    "鉂?" = "ℹ️"
    
    # Fix hex-encoded characters
    "<e2><a0>" = "⠋"
    "<e2><94>" = "┌"
    "<e2><95>" = "╔"
    "<U+2500>" = "─"
}

# Get all source files
$sourceFiles = Get-ChildItem -Path "src" -Recurse -Include "*.cpp", "*.h", "*.hpp" | Where-Object { $_.Name -notlike "*.bak" }

$totalFiles = $sourceFiles.Count
$processedFiles = 0

Write-Host "Found $totalFiles source files to process" -ForegroundColor Yellow

foreach ($file in $sourceFiles) {
    $processedFiles++
    Write-Progress -Activity "Fixing UTF-8 encoding" -Status "Processing $($file.Name)" -PercentComplete (($processedFiles / $totalFiles) * 100)
    
    try {
        # Read file content with UTF-8 encoding
        $content = Get-Content -Path $file.FullName -Raw -Encoding UTF8
        $originalContent = $content
        
        # Apply all UTF-8 fixes
        foreach ($fix in $utf8Fixes.GetEnumerator()) {
            $content = $content -replace [regex]::Escape($fix.Key), $fix.Value
        }
        
        # Only write if content changed
        if ($content -ne $originalContent) {
            # Create backup
            Copy-Item -Path $file.FullName -Destination "$($file.FullName).bak" -Force
            
            # Write fixed content
            Set-Content -Path $file.FullName -Value $content -Encoding UTF8 -NoNewline
            Write-Host "  Fixed: $($file.Name)" -ForegroundColor Green
        }
    }
    catch {
        Write-Host "  Error processing $($file.Name): $($_.Exception.Message)" -ForegroundColor Red
    }
}

Write-Progress -Activity "Fixing UTF-8 encoding" -Completed

Write-Host "`nUTF-8 encoding fix completed!" -ForegroundColor Green
Write-Host "Backup files created with .bak extension" -ForegroundColor Yellow
