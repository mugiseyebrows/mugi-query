@echo off
rem This file is generated from build-lld.pbat, all edits will be lost
set PATH=C:\Program Files\CMake\bin;C:\Program Files\Meson;C:\mingw1120_64\bin;C:\qwt\6.3.0\mingw1120_64\lib;C:\Miniconda3;C:\Miniconda3\Scripts;%USERPROFILE%\Miniconda3;%USERPROFILE%\Miniconda3\Scripts;C:\LLVM-18\bin;%PATH%
pushd %~dp0
    if not exist debug mkdir debug
    if not exist release mkdir release
    pushd debug
        cmake -G Ninja -DCMAKE_CXX_FLAGS="-fuse-ld=lld" -D CMAKE_BUILD_TYPE=Debug -D CMAKE_PREFIX_PATH=C:/Qt/6.7.1/mingw1120_64;C:/qwt/6.3.0/mingw1120_64 ..
        cmake --build .
    popd
    pushd release
        cmake -G Ninja -DCMAKE_CXX_FLAGS="-fuse-ld=lld" -D CMAKE_BUILD_TYPE=Release -D CMAKE_PREFIX_PATH=C:/Qt/6.7.1/mingw1120_64;C:/qwt/6.3.0/mingw1120_64 ..
        cmake --build .
    popd
popd