@echo off
setlocal enabledelayedexpansion

:: Check if argument is provided
if "%1"=="" (
    echo Usage: build.bat [dll^|static]
    echo   dll    - Build dynamic library ^(.dll^)
    echo   static - Build static library ^(.a^)
    exit /b 1
)

:: Set paths for curl
set CURL_PATH=C:\curl
set CURL_INCLUDE=%CURL_PATH%\include
set CURL_LIB=%CURL_PATH%\lib

:: Set compiler and flags
set CC=gcc
set CFLAGS=-Wall -Wextra -I. -I%CURL_INCLUDE% -DCURL_STATICLIB
set LIBS=-L%CURL_LIB% -lcurl -lws2_32 -lwinmm -lwldap32 -lcrypt32
set OUTDIR=build

:: Check if curl paths exist
if not exist "%CURL_PATH%" (
    echo Error: Curl directory not found at %CURL_PATH%
    echo Please install curl or update CURL_PATH in the build script
    exit /b 1
)

:: Create build directory if it doesn't exist
if not exist %OUTDIR% mkdir %OUTDIR%

:: Clean previous builds
del /Q %OUTDIR%\*.*

:: Build based on argument
if /i "%1"=="dll" (
    echo Building DLL...
    %CC% -shared -o %OUTDIR%\neocities.dll neocities.c %CFLAGS% -Wl,--out-implib,%OUTDIR%\libneocities.a %LIBS%
    if !errorlevel! equ 0 (
        echo DLL built successfully: %OUTDIR%\neocities.dll
        echo Import library created: %OUTDIR%\libneocities.a
    )
) else if /i "%1"=="static" (
    echo Building static library...
    %CC% -c neocities.c %CFLAGS%
    ar rcs %OUTDIR%\libneocities.a neocities.o
    del neocities.o
    if !errorlevel! equ 0 (
        echo Static library built successfully: %OUTDIR%\libneocities.a
    )
) else (
    echo Invalid argument: %1
    echo Use 'dll' or 'static'
    exit /b 1
)

endlocal 