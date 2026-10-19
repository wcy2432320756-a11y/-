param([switch]$SkipCompile)

# 教研组内部用的机械筛查脚本 —— 只查能程序化检查的东西，
# 内容质量仍然要人工看。
# 用法： .\verify.ps1            检查全部三章并编译
#        .\verify.ps1 -SkipCompile  只做静态检查

$ErrorActionPreference = 'Continue'
$root = Split-Path -Parent $MyInvocation.MyCommand.Path

$chapters = @(
    @{ N = '01'; Md = '讲义\01-类与对象.md';      Cpp = 'lesson01.cpp' },
    @{ N = '02'; Md = '讲义\02-组合与依赖.md';    Cpp = 'lesson02.cpp' },
    @{ N = '03'; Md = '讲义\03-继承与派生.md';    Cpp = 'lesson03.cpp' },
    @{ N = '04'; Md = '讲义\04-多态与虚函数.md';  Cpp = 'lesson04.cpp' }
)

# 讲义骨架必须出现的小节标志
$skeleton = @('一、', '八、', '九、', '十、动手任务')

# 明确的占位词（保守，避免误伤正常用词）
$placeholders = @('自行补充', '以此类推', 'TODO', '待补', '（略）', '略去不写',
                  '其余同上', '具体实现省略', '这里省略')

function Test-File {
    param([string]$Path)
    $full = Join-Path $root $Path
    if (-not (Test-Path $full)) { return @{ Ok = $false; Msg = 'MISSING' } }
    $text = Get-Content $full -Raw -Encoding UTF8
    $problems = @()

    if ($Path -like '*.md') {
        foreach ($s in $skeleton) {
            if ($text -notmatch [regex]::Escape($s)) { $problems += "no-section:$s" }
        }
        foreach ($p in $placeholders) {
            if ($text -match [regex]::Escape($p)) { $problems += "placeholder:$p" }
        }
        # emoji：代理对区间，真正的 emoji 几乎都在这里
        if ($text -match '[\uD800-\uDBFF]') { $problems += 'emoji-or-surrogate' }
        # 长度门槛：第一课约 1.1 万字符，要求不低于 6000
        if ($text.Length -lt 6000) { $problems += "too-short:$($text.Length)chars" }
    }

    if ($Path -like '*.cpp') {
        if ($text -notmatch 'SetConsoleOutputCP') { $problems += 'no-console-cp' }
        if ($text -notmatch '\bint\s+main\s*\(') { $problems += 'no-main' }
        foreach ($p in $placeholders) {
            if ($text -match [regex]::Escape($p)) { $problems += "placeholder:$p" }
        }
    }

    if ($problems.Count -eq 0) { return @{ Ok = $true; Msg = 'OK' } }
    return @{ Ok = $false; Msg = ($problems -join ' | ') }
}

$fail = 0
foreach ($c in $chapters) {
    Write-Host ""
    Write-Host "=== chapter $($c.N) ===" -ForegroundColor Cyan

    foreach ($item in @(@{ Kind = 'md'; Path = $c.Md }, @{ Kind = 'cpp'; Path = $c.Cpp })) {
        $r = Test-File -Path $item.Path
        if ($r.Ok) {
            Write-Host ("  {0,-4} {1,-28} PASS" -f $item.Kind, $item.Path) -ForegroundColor Green
        } else {
            $fail++
            Write-Host ("  {0,-4} {1,-28} FAIL  {2}" -f $item.Kind, $item.Path, $r.Msg) -ForegroundColor Red
        }
    }

    if ((-not $SkipCompile) -and (Test-Path (Join-Path $root $c.Cpp))) {
        $less = 'lesson' + $c.N
        $out = & (Join-Path $root 'build.ps1') $less -NoRun 2>&1 | Out-String
        if ($out -match 'BUILD OK') {
            Write-Host ("  {0,-4} {1,-28} PASS" -f 'build', $less) -ForegroundColor Green
        } else {
            $fail++
            Write-Host ("  {0,-4} {1,-28} FAIL" -f 'build', $less) -ForegroundColor Red
        }
    }
}

Write-Host ""
if ($fail -eq 0) {
    Write-Host "ALL CHECKS PASSED" -ForegroundColor Green
} else {
    Write-Host "FAILED ITEMS: $fail" -ForegroundColor Yellow
}
