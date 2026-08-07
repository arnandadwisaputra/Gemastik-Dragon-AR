@echo off
setlocal enabledelayedexpansion
title Dragon Asteroid Run - Auto Builder

echo ==================================================
echo   Dragon Asteroid Run - Auto Build & Run Script
echo ==================================================
echo.

:: 1. Deteksi lokasi CMake bawaan Visual Studio / Build Tools
set "CMAKE_PATH="

:: Daftar lokasi umum instalasi CMake oleh Visual Studio 2022
set "PATHS[0]=C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "PATHS[1]=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "PATHS[2]=C:\Program Files\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "PATHS[3]=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

for /L %%i in (0,1,3) do (
    if not defined CMAKE_PATH (
        if exist "!PATHS[%%i]!" (
            set "CMAKE_PATH=!PATHS[%%i]!"
            echo [+] Menemukan CMake di: !CMAKE_PATH!
        )
    )
)

:: Jika tidak ditemukan di lokasi standar VS, coba panggil 'cmake' global
if not defined CMAKE_PATH (
    where cmake >nul 2>nul
    if !errorlevel! equ 0 (
        set "CMAKE_PATH=cmake"
        echo [+] Menemukan CMake global di PATH.
    )
)

:: Jika tetap tidak ditemukan
if not defined CMAKE_PATH (
    echo [ERROR] CMake / Visual Studio Build Tools tidak ditemukan!
    echo Silakan instal C++ Build Tools terlebih dahulu.
    echo Unduh dari: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022
    echo.
    pause
    exit /b 1
)

echo.
echo [1/3] Mengonfigurasi CMake...
echo --------------------------------------------------
"!CMAKE_PATH!" -B build -S . -G "Visual Studio 17 2022" -A x64
if !errorlevel! neq 0 (
    echo.
    echo [ERROR] Gagal mengonfigurasi CMake.
    pause
    exit /b !errorlevel!
)

echo.
echo [2/3] Melakukan Compile Game (Build)...
echo --------------------------------------------------
"!CMAKE_PATH!" --build build --config Release
if !errorlevel! neq 0 (
    echo.
    echo [ERROR] Gagal melakukan compile game.
    pause
    exit /b !errorlevel!
)

echo.
echo [3/3] Sukses! Menjalankan Game...
echo --------------------------------------------------
cd build\Release
start DragonAsteroidRun.exe
echo Game telah berjalan. Anda bisa menutup jendela terminal ini.
echo.
exit /b 0
