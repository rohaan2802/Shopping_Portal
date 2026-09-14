@echo off
setlocal EnableExtensions
cd /d "%~dp0"

REM Always rebuild so source updates (UI, padding, stock, etc.) are not skipped.
echo Building latest sources before run...
call "%~dp0build.bat"
if errorlevel 1 (
  echo [ERROR] Build failed - cannot run.
  pause
  exit /b 1
)

if not exist "build\Shopping_Portal.exe" (
  echo [ERROR] build\Shopping_Portal.exe not found after build.
  pause
  exit /b 1
)

echo.
echo Starting FAST Shopping Portal in a new console window...
echo Working directory: %cd%
echo.

REM External console: colors / cls / _getch work reliably
start "FAST Shopping Portal" /D "%~dp0" cmd /k "build\Shopping_Portal.exe"

endlocal
exit /b 0
