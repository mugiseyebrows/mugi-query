@echo off
rem This file is generated from build-msys2.pbat, all edits will be lost
set PATH=C:\msys64\ucrt64\bin;C:\Program Files\CMake\bin;%LOCALAPPDATA%\Programs\Python\Python311;%LOCALAPPDATA%\Programs\Python\Python311\Scripts;C:\Python311;C:\Python311\Scripts;C:\Program Files\Meson;%PATH%
set BUILD_TYPE=Release
if /I "%1"=="debug" set BUILD_TYPE=Debug
echo BUILD_TYPE %BUILD_TYPE%
pushd %~dp0
    if not exist "%BUILD_TYPE%-msys2" mkdir "%BUILD_TYPE%-msys2"
    pushd %BUILD_TYPE%-msys2
        cmake -G Ninja -D CMAKE_BUILD_TYPE=%BUILD_TYPE% -D CMAKE_PREFIX_PATH=C:/msys64/ucrt64;C:/msys64/ucrt64/include/qwt-qt6 ..
        cmake --build .
    popd
popd