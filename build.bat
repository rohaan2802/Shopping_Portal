@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo ============================================
echo  FAST Shopping Portal - MSVC Build
echo ============================================

set "VCVARS="
if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" (
  for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
    if exist "%%i\VC\Auxiliary\Build\vcvars64.bat" set "VCVARS=%%i\VC\Auxiliary\Build\vcvars64.bat"
  )
)

if not defined VCVARS (
  if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
  if exist "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
  if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
)

if not defined VCVARS (
  echo [ERROR] Could not find vcvars64.bat. Install Visual Studio C++ tools.
  exit /b 1
)

call "%VCVARS%" >nul
if errorlevel 1 (
  echo [ERROR] Failed to load MSVC environment.
  exit /b 1
)

if not exist build mkdir build

echo Compiling...
cl /nologo /EHsc /std:c++17 /W3 /D_HAS_STD_BYTE=0 /DNOMINMAX /DWIN32_LEAN_AND_MEAN ^
  /Fe:build\Shopping_Portal.exe ^
  Main.cpp Admin.cpp Vendor.cpp Customer.cpp Items.cpp Cart.cpp ^
  /link User32.lib

if errorlevel 1 (
  echo [ERROR] Build failed.
  exit /b 1
)

echo.
echo [OK] Built: build\Shopping_Portal.exe
endlocal
exit /b 0
