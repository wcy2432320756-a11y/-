param(
    [Parameter(Position = 0)][string]$Name = "lesson01",
    [switch]$NoRun
)

$ErrorActionPreference = 'Stop'
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
$src  = Join-Path $here "$Name.cpp"

if (-not (Test-Path $src)) {
    Write-Host "Source not found: $src" -ForegroundColor Red
    exit 1
}

$vcvars = "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
if (-not (Test-Path $vcvars)) {
    Write-Host "vcvars64.bat not found: $vcvars" -ForegroundColor Red
    exit 1
}

$outDir = Join-Path $here "bin"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$exe = Join-Path $outDir "$Name.exe"

$line = "`"$vcvars`" >nul 2>&1 && cl /nologo /EHsc /std:c++17 /utf-8 /W3 /Fo:`"$outDir\\`" /Fe:`"$exe`" `"$src`""
cmd /c $line

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "BUILD FAILED - read the error above, it names the line." -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host ""
Write-Host "BUILD OK: $exe" -ForegroundColor Green

if (-not $NoRun) {
    Write-Host "---------- OUTPUT ----------" -ForegroundColor Cyan
    & $exe
}
