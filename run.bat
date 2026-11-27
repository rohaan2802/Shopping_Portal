@echo off
setlocal EnableExtensions
cd /d "%~dp0"

if not exist "build\Shopping_Portal.exe" if not exist "build\Release\Shopping_Portal.exe" (
  echo [INFO] No native executable was found. Building the project first...
  call "%~dp0build.bat"
  if errorlevel 1 (
    echo [ERROR] The build failed, so the portal was not started.
    exit /b 1
  )
)

python tools\prepare_demo.py
if errorlevel 1 (
  echo [ERROR] Runtime demo data could not be prepared.
  exit /b 1
)

set "PORTAL_EXE="
if exist "build\Shopping_Portal.exe" set "PORTAL_EXE=%CD%\build\Shopping_Portal.exe"
if not defined PORTAL_EXE if exist "build\Release\Shopping_Portal.exe" set "PORTAL_EXE=%CD%\build\Release\Shopping_Portal.exe"
if not defined PORTAL_EXE (
  echo [ERROR] Build completed but Shopping_Portal.exe was not found.
  exit /b 1
)

pushd data\runtime
echo [INFO] Starting Shopping_Portal with isolated runtime data...
"%PORTAL_EXE%"
set "PORTAL_EXIT=%ERRORLEVEL%"
popd
exit /b %PORTAL_EXIT%
