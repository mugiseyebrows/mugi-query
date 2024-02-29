@echo off
rem This file is generated from build.pbat, all edits will be lost
set PATH=C:\Program Files\PostgreSQL\14\bin;C:\Qwt-6.2.0\lib;C:\mysql-8.2.0-winx64\bin;C:\mysql-8.2.0-winx64\lib;C:\Miniconda3;C:\Miniconda3\Scripts;%USERPROFILE%\Miniconda3;%USERPROFILE%\Miniconda3\Scripts;C:\Qt\5.15.2\mingw81_64\bin;C:\Qt\Tools\mingw810_64\bin;%PATH%
if exist "C:\Program Files\7-Zip\7z.exe" set P7Z=C:\Program Files\7-Zip\7z.exe
if not defined P7Z (
echo P7Z not found
exit /b
)
where aqt || pip install aqtinstall
if not exist "C:\Qt\5.15.2\mingw81_64\bin\qmake.exe" aqt install-qt windows desktop 5.15.2 win64_mingw81 -O C:\Qt
if not exist "C:\Qt\Tools\mingw810_64\bin\gcc.exe" aqt install-tool windows desktop tools_mingw qt.tools.win64_mingw810 -O C:\Qt
where pyfind || pip install mugicli
where mugideploy || pip install mugideploy
if not exist "C:\Qt\5.15.2\mingw81_64\plugins\sqldrivers\qsqlmysql.dll" call install-qsqlmysql
if not exist "C:\Qwt-6.2.0\lib\qwt.dll" call install-qwt
if not exist "C:\mysql-8.2.0-winx64\bin\mysql.exe" call install-mysql
echo building mugi-query
mugideploy version
qmake
mingw32-make release -j4
mugideploy collect --app mugi-query --git-version --bin release\mugi-query.exe --plugins odbc mysql sqlite psql --zip