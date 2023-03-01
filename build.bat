@echo off
rem This file is generated from build.pbat, all edits will be lost
if exist "C:\Program Files\Git\mingw32\bin\curl.exe" set CURL=C:\Program Files\Git\mingw32\bin\curl.exe
if exist "C:\Program Files\Git\mingw64\bin\curl.exe" set CURL=C:\Program Files\Git\mingw64\bin\curl.exe
if exist "C:\Windows\System32\curl.exe" set CURL=C:\Windows\System32\curl.exe
if not defined CURL (
echo CURL not found
exit /b
)
set PATH=C:\Miniconda3;C:\Miniconda3\Scripts;%PATH%
pip install mugicli
pip install aqtinstall
aqt install-tool windows desktop tools_mingw qt.tools.win64_mingw810 -O C:\Qt
aqt install-qt windows desktop 5.15.2 win64_mingw81 -O C:\Qt
pyfind C:\Qt -iname qmake.exe gcc.exe qsqlpsql.dll
if not exist qsqlmysql-qt5.15.2-mingw8.1.0-x86_64.zip "%CURL%" -L -o qsqlmysql-qt5.15.2-mingw8.1.0-x86_64.zip https://github.com/mugiseyebrows/qsqlmysql-builds/releases/download/5.15.2/qsqlmysql-qt5.15.2-mingw8.1.0-x86_64.zip
"C:\Program Files\Git\usr\bin\file.exe" C:\mysql\bin\libmysql.dll