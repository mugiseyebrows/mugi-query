@echo off
rem This file is generated from build.pbat, all edits will be lost
set PATH=C:\Program Files\CMake\bin;%LOCALAPPDATA%\Programs\Python\Python311;%LOCALAPPDATA%\Programs\Python\Python311\Scripts;C:\Python311;C:\Python311\Scripts;C:\Program Files\Meson;C:\mingw1120_64\bin;C:\Qt\6.7.1\mingw1120_64\bin;C:\qwt\6.3.0\mingw1120_64\lib;C:\LLVM-18\bin;%PATH%
set BUILD_TYPE=Release
if /I "%1"=="debug" set BUILD_TYPE=Debug
echo BUILD_TYPE %BUILD_TYPE%
pushd %~dp0
    if not exist "%BUILD_TYPE%" mkdir "%BUILD_TYPE%"
    pushd %BUILD_TYPE%
        cmake -G Ninja -D CMAKE_BUILD_TYPE=%BUILD_TYPE% -D CMAKE_PREFIX_PATH=C:/Qt/6.7.1/mingw1120_64;C:/qwt/6.3.0/mingw1120_64 ..
        cmake --build .
    popd
popd