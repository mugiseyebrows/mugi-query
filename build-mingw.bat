@echo off
rem This file is generated from build-mingw.pbat, all edits will be lost
set PATH=C:\Windows\System32;C:\Program Files\7-Zip;C:\Qt\6.10.0\mingw_64\bin;C:\Qwt-6.3.1-Qt-6.10.0\lib;C:\mingw1310_64\bin;C:\Miniconda3;C:\Miniconda3\Scripts;%USERPROFILE%\Miniconda3;%USERPROFILE%\Miniconda3\Scripts;%PATH%
if exist C:\mingw1310_64\bin\gcc.exe goto mingw_end
if not exist winlibs-x86_64-posix-seh-gcc-13.1.0-mingw-w64ucrt-11.0.0-r5.7z (
    echo downloading winlibs-x86_64-posix-seh-gcc-13.1.0-mingw-w64ucrt-11.0.0-r5.7z
    curl -L -o winlibs-x86_64-posix-seh-gcc-13.1.0-mingw-w64ucrt-11.0.0-r5.7z https://github.com/brechtsanders/winlibs_mingw/releases/download/13.1.0-16.0.5-11.0.0-ucrt-r5/winlibs-x86_64-posix-seh-gcc-13.1.0-mingw-w64ucrt-11.0.0-r5.7z
)
7z rn winlibs-x86_64-posix-seh-gcc-13.1.0-mingw-w64ucrt-11.0.0-r5.7z mingw64 mingw1310_64
if not exist C:\mingw1310_64\bin\gcc.exe 7z x -y -oC:\ winlibs-x86_64-posix-seh-gcc-13.1.0-mingw-w64ucrt-11.0.0-r5.7z
:mingw_end
if exist C:\Qt\6.10.0\mingw_64\bin\qmake.exe goto qt_end
if not exist Qt-6.10.0-mingw13.7z (
    echo downloading Qt-6.10.0-mingw13.7z
    curl -L -o Qt-6.10.0-mingw13.7z https://github.com/mugiseyebrows/build-qt/releases/download/6.10.0/Qt-6.10.0-mingw13.7z
)
7z x -y -oC:\Qt\6.10.0 Qt-6.10.0-mingw13.7z
:qt_end
if exist C:\Qwt-6.3.1-Qt-6.10.0\lib\qwt.dll goto qwt_end
if not exist Qwt-6.3.1-Qt-6.10.0-mingw13.7z (
    echo downloading Qwt-6.3.1-Qt-6.10.0-mingw13.7z
    curl -L -o Qwt-6.3.1-Qt-6.10.0-mingw13.7z https://github.com/mugiseyebrows/build-qwt/releases/download/6.3.1/Qwt-6.3.1-Qt-6.10.0-mingw13.7z
)
7z x -y -oC:\ Qwt-6.3.1-Qt-6.10.0-mingw13.7z
:qwt_end
if not exist Release mkdir Release
pushd Release
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/Qwt-6.3.1-Qt-6.10.0 ..
    ninja
popd
where mugideploy > NUL 2>&1 || pip install mugideploy
mugideploy collect --bin Release\mugi-query.exe --data emmet.json --plugins sqldrivers --dst "[name]-[version]-[arch]-mingw13" --zip