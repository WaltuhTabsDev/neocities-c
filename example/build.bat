@echo off
setlocal enabledelayedexpansion

:: Set paths
set OUTDIR=build
set LIBDIR=..\build
set CURL_PATH=C:\curl
set CURL_INCLUDE=%CURL_PATH%\include
set CURL_LIB=%CURL_PATH%\lib

:: Check if curl paths exist
if not exist "%CURL_PATH%" (
    echo Error: Curl directory not found at %CURL_PATH%
    echo Please install curl or update CURL_PATH in the build script
    exit /b 1
)

:: Create build directory if it doesn't exist
if not exist %OUTDIR% mkdir %OUTDIR%

:: Check if library exists
if not exist %LIBDIR%\neocities.dll (
    echo Error: neocities.dll not found. Please build the library first.
    echo Run 'build.bat dll' in the parent directory.
    exit /b 1
)

:: Copy DLL to example build directory
copy %LIBDIR%\neocities.dll %OUTDIR%\

:: Compile the example
gcc -o %OUTDIR%\example.exe main.c ^
    -I.. ^
    -I%CURL_INCLUDE% ^
    -L%LIBDIR% ^
    -L%CURL_LIB% ^
    -lneocities ^
    -Wl,--subsystem,console ^
    -municode

if !errorlevel! equ 0 (
    echo Build successful! Run %OUTDIR%\example.exe to test.
) else (
    echo Build failed!
)

endlocal 