@echo off
setlocal EnableExtensions
cd /d "%~dp0"
where cmake >nul 2>nul
if errorlevel 1 (
  echo [ERROR] CMake was not found. Install CMake 3.20 or later, then run this file again.
  exit /b 1
)

if exist "build\CMakeCache.txt" (
  echo [INFO] Reusing the existing CMake build configuration.
  cmake -S . -B build
) else (
  where mingw32-make >nul 2>nul
  if not errorlevel 1 (
    echo [INFO] Configuring a Release build with MinGW Makefiles.
    cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
  ) else (
    echo [INFO] Configuring with CMake's default Windows generator.
    cmake -S . -B build
  )
)
if errorlevel 1 exit /b 1

cmake --build build --config Release
if errorlevel 1 exit /b 1

ctest --test-dir build -C Release --output-on-failure
if errorlevel 1 exit /b 1

echo [OK] Build and CTest checks completed.
