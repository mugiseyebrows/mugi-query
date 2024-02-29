@echo off
rem This file is generated from install-qsqlmysql.pbat, all edits will be lost
set PATH=C:\Program Files\7-Zip;%PATH%
if exist "C:\Program Files\Git\mingw32\bin\curl.exe" set CURL=C:\Program Files\Git\mingw32\bin\curl.exe
if exist "C:\Program Files\Git\mingw64\bin\curl.exe" set CURL=C:\Program Files\Git\mingw64\bin\curl.exe
if exist "C:\Windows\System32\curl.exe" set CURL=C:\Windows\System32\curl.exe
if not defined CURL (
echo CURL not found
exit /b
)
echo downloading qsqlmysql
if not exist qsqlmysql-qt5.15.2-mingw8.1.0-x86_64.zip "%CURL%" -L -o qsqlmysql-qt5.15.2-mingw8.1.0-x86_64.zip https://github.com/mugiseyebrows/qsqlmysql-builds/releases/download/5.15.2/qsqlmysql-qt5.15.2-mingw8.1.0-x86_64.zip
echo installing qsqlmysql
if not exist qsqlmysql-qt5.15.2-mingw8.1.0-x86_64 7z x -y qsqlmysql-qt5.15.2-mingw8.1.0-x86_64.zip
copy /y qsqlmysql-qt5.15.2-mingw8.1.0-x86_64\qsqlmysql.dll C:\Qt\5.15.2\mingw81_64\plugins\sqldrivers
copy /y qsqlmysql-qt5.15.2-mingw8.1.0-x86_64\qsqlmysqld.dll C:\Qt\5.15.2\mingw81_64\plugins\sqldrivers