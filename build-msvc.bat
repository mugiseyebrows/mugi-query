@echo off
rem This file is generated from build-msvc.pbat, all edits will be lost
set PATH=C:\Windows\System32;C:\Program Files\7-Zip;C:\Qt\6.10.0\msvc2020_64\bin;C:\Qwt-6.3.1-Qt-6.10.0\lib;C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build;C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build;C:\Miniconda3;C:\Miniconda3\Scripts;%USERPROFILE%\Miniconda3;%USERPROFILE%\Miniconda3\Scripts;%PATH%
echo 1
if exist C:\Qt\6.10.0\msvc2020_64\bin\qmake.exe goto qt_end
if not exist Qt-6.10.0-msvc2020.7z (
    echo downloading Qt-6.10.0-msvc2020.7z
    curl -L -o Qt-6.10.0-msvc2020.7z https://github.com/mugiseyebrows/build-qt/releases/download/6.10.0/Qt-6.10.0-msvc2020.7z
)
7z x -y -oC:\Qt\6.10.0 Qt-6.10.0-msvc2020.7z
:qt_end
if exist C:\Qwt-6.3.1-Qt-6.10.0\lib\qwt.dll goto qwt_end
if not exist Qwt-6.3.1-Qt-6.10.0-msvc2020.7z (
    echo downloading Qwt-6.3.1-Qt-6.10.0-msvc2020.7z
    curl -L -o Qwt-6.3.1-Qt-6.10.0-msvc2020.7z https://github.com/mugiseyebrows/build-qwt/releases/download/6.3.1/Qwt-6.3.1-Qt-6.10.0-msvc2020.7z
)
7z x -y -oC:\ Qwt-6.3.1-Qt-6.10.0-msvc2020.7z
:qwt_end
call vcvars64.bat
if not exist Release mkdir Release
pushd Release
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/Qwt-6.3.1-Qt-6.10.0 ..
    ninja
popd
call vcvars64.bat
where mugideploy > NUL 2>&1 || pip install mugideploy
mugideploy collect --bin Release\mugi-query.exe --data emmet.json --plugins sqldrivers --dst "[name]-[version]-[arch]-msvc2020" --zip