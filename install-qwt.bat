@echo off
rem This file is generated from install-qwt.pbat, all edits will be lost
set PATH=C:\Program Files\7-Zip;%PATH%
if exist "C:\Program Files\Git\mingw32\bin\curl.exe" set CURL=C:\Program Files\Git\mingw32\bin\curl.exe
if exist "C:\Program Files\Git\mingw64\bin\curl.exe" set CURL=C:\Program Files\Git\mingw64\bin\curl.exe
if exist "C:\Windows\System32\curl.exe" set CURL=C:\Windows\System32\curl.exe
if not defined CURL (
echo CURL not found
exit /b
)
echo downloading qwt
if not exist qwt6.2.0-qt5.15.2-mingw8.1.0-x86_64.zip "%CURL%" -L -o qwt6.2.0-qt5.15.2-mingw8.1.0-x86_64.zip https://github.com/mugiseyebrows/qwt-builds/releases/download/qwt6.2.0-qt5.15.2/qwt6.2.0-qt5.15.2-mingw8.1.0-x86_64.zip
echo installing qwt
7z x -y -oC:\ qwt6.2.0-qt5.15.2-mingw8.1.0-x86_64.zip