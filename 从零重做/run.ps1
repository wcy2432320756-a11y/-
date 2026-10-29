# ============================================================
# run.ps1 -- build + run one lesson  (ASCII only: no encoding traps)
# Usage:  .\run.ps1 step01             build and run step01.cpp
#         .\run.ps1 step01 -NoRun      build only, do not run
# ============================================================
param(
    [Parameter(Position = 0)][string]$Name = "step01",
    [switch]$NoRun
)

$ErrorActionPreference = 'Stop'
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
$src  = Join-Path $here "$Name.cpp"

if (-not (Test-Path $src)) {
    Write-Host "source not found: $src" -ForegroundColor Red
    exit 1
}

$vcvars = "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
if (-not (Test-Path $vcvars)) {
    Write-Host "compiler env not found: $vcvars" -ForegroundColor Red
    exit 1
}

$outDir = Join-Path $here "_build"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$exe = Join-Path $outDir "$Name.exe"

# ---- build (cl = MSVC compiler). Sources are GBK: no /utf-8 needed. ----
$line = "`"$vcvars`" >nul 2>&1 && cl /nologo /EHsc /std:c++14 /W3 /Fo:`"$outDir\\`" /Fe:`"$exe`" `"$src`""
cmd /c $line

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "BUILD FAILED -- read the first error above; it shows the line number." -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host ""
Write-Host "BUILD OK: $exe" -ForegroundColor Green

# ---- run (console codepage 936 = GBK, matches the GBK bytes we print) ----
if (-not $NoRun) {
    Write-Host "---------- RUN ----------" -ForegroundColor Cyan
    chcp.com 936 | Out-Null
    & $exe
}