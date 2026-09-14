@echo off
setlocal
cd /d "%~dp0"

if not exist build\Shopping_Portal.exe (
  echo Executable missing — running build.bat first...
  call build.bat
  if errorlevel 1 exit /b 1
)

echo Starting FAST Shopping Portal...
echo Working directory: %cd%
echo.
build\Shopping_Portal.exe
endlocal
