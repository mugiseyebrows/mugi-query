@echo off
rem This file is generated from build.pbat, all edits will be lost
set PATH=C:\Program Files\CMake\bin;C:\Program Files\Meson;%LOCALAPPDATA%\Programs\Python\Python38;%LOCALAPPDATA%\Programs\Python\Python38\Scripts;C:\Python38;C:\Python38\Scripts;%LOCALAPPDATA%\Programs\Python\Python39;%LOCALAPPDATA%\Programs\Python\Python39\Scripts;C:\Python39;C:\Python39\Scripts;%LOCALAPPDATA%\Programs\Python\Python310;%LOCALAPPDATA%\Programs\Python\Python310\Scripts;C:\Python310;C:\Python310\Scripts;%LOCALAPPDATA%\Programs\Python\Python311;%LOCALAPPDATA%\Programs\Python\Python311\Scripts;C:\Python311;C:\Python311\Scripts;%LOCALAPPDATA%\Programs\Python\Python312;%LOCALAPPDATA%\Programs\Python\Python312\Scripts;C:\Python312;C:\Python312\Scripts;%LOCALAPPDATA%\Programs\Python\Python313;%LOCALAPPDATA%\Programs\Python\Python313\Scripts;C:\Python313;C:\Python313\Scripts;%LOCALAPPDATA%\Programs\Python\Python314;%LOCALAPPDATA%\Programs\Python\Python314\Scripts;C:\Python314;C:\Python314\Scripts;C:\mingw1120_64\bin;C:\Qt\6.7.1\mingw1120_64;C:\qwt\6.3.0\mingw1120_64\lib;C:\LLVM-18\bin;%PATH%
pushd %~dp0
    if not exist debug mkdir debug
    if not exist release mkdir release
    pushd debug
        cmake -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_PREFIX_PATH=C:/Qt/6.7.1/mingw1120_64;C:/qwt/6.3.0/mingw1120_64 ..
        cmake --build .
    popd
    pushd release
        cmake -G Ninja -D CMAKE_BUILD_TYPE=Release -D CMAKE_PREFIX_PATH=C:/Qt/6.7.1/mingw1120_64;C:/qwt/6.3.0/mingw1120_64 ..
        cmake --build .
    popd
popd