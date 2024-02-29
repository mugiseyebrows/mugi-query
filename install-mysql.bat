@echo off
rem This file is generated from install-mysql.pbat, all edits will be lost
set PATH=C:\Program Files\7-Zip;%PATH%
if exist "C:\Program Files\Git\mingw32\bin\curl.exe" set CURL=C:\Program Files\Git\mingw32\bin\curl.exe
if exist "C:\Program Files\Git\mingw64\bin\curl.exe" set CURL=C:\Program Files\Git\mingw64\bin\curl.exe
if exist "C:\Windows\System32\curl.exe" set CURL=C:\Windows\System32\curl.exe
if not defined CURL (
echo CURL not found
exit /b
)
echo downloading mysql-8.2.0
if not exist mysql-8.2.0-winx64.zip "%CURL%" -L -o mysql-8.2.0-winx64.zip https://cdn.mysql.com/Downloads/MySQL-8.2/mysql-8.2.0-winx64.zip
echo installing mysql-8.2.0
7z x -y -oC:\ mysql-8.2.0-winx64.zip