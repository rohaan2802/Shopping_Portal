@echo off
setlocal EnableExtensions
cd /d "%~dp0"

REM Always delete old exe + rebuild so UI/alignment updates are never skipped.
echo Cleaning previous build...
if exist "build\Shopping_Portal.exe" del /f /q "build\Shopping_Portal.exe"
if exist "build\*.obj" del /f /q "build\*.obj" 2>nul
if exist "*.obj" del /f /q "*.obj" 2>nul

echo Building latest sources...
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

start "FAST Shopping Portal" /D "%~dp0" cmd /k "build\Shopping_Portal.exe"

endlocal
exit /b 0
