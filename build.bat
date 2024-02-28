@echo off
rem This file is generated from build.pbat, all edits will be lost
set PATH=C:\Program Files\PostgreSQL\14\bin;C:\Qwt-6.2.0\lib;C:\mysql-8.2.0-winx64\bin;C:\mysql-8.2.0-winx64\lib;C:\Miniconda3;C:\Miniconda3\Scripts;%USERPROFILE%\Miniconda3;%USERPROFILE%\Miniconda3\Scripts;C:\Qt\5.15.2\mingw81_64\bin;C:\Qt\Tools\mingw810_64\bin;%PATH%
if exist "C:\Program Files\7-Zip\7z.exe" set P7Z=C:\Program Files\7-Zip\7z.exe
if not defined P7Z (
echo P7Z not found
exit /b
)
if exist "C:\Program Files\Git\mingw32\bin\curl.exe" set CURL=C:\Program Files\Git\mingw32\bin\curl.exe
if exist "C:\Program Files\Git\mingw64\bin\curl.exe" set CURL=C:\Program Files\Git\mingw64\bin\curl.exe
if exist "C:\Windows\System32\curl.exe" set CURL=C:\Windows\System32\curl.exe
if not defined CURL (
echo CURL not found
exit /b
)
where aqt || pip install aqtinstall
if not exist "C:\Qt\5.15.2\mingw81_64\bin\qmake.exe" aqt install-qt windows desktop 5.15.2 win64_mingw81 -O C:\Qt
if not exist "C:\Qt\Tools\mingw810_64\bin\gcc.exe" aqt install-tool windows desktop tools_mingw qt.tools.win64_mingw810 -O C:\Qt
where pyfind || pip install mugicli
where mugideploy || pip install mugideploy
if exist "C:\Qt\5.15.2\mingw81_64\plugins\sqldrivers\qsqlmysql.dll" goto qsqlmysql_end
if not exist qsqlmysql-qt5.15.2-mingw8.1.0-x86_64.zip "%CURL%" -L -o qsqlmysql-qt5.15.2-mingw8.1.0-x86_64.zip https://github.com/mugiseyebrows/qsqlmysql-builds/releases/download/5.15.2/qsqlmysql-qt5.15.2-mingw8.1.0-x86_64.zip
if not exist qsqlmysql-qt5.15.2-mingw8.1.0-x86_64 "%P7Z%" x -y qsqlmysql-qt5.15.2-mingw8.1.0-x86_64.zip
copy /y qsqlmysql-qt5.15.2-mingw8.1.0-x86_64\qsqlmysql.dll C:\Qt\5.15.2\mingw81_64\plugins\sqldrivers
copy /y qsqlmysql-qt5.15.2-mingw8.1.0-x86_64\qsqlmysqld.dll C:\Qt\5.15.2\mingw81_64\plugins\sqldrivers
:qsqlmysql_end
if exist "C:\Qwt-6.2.0\lib\qwt.dll" goto qwt_end
if not exist qwt6.2.0-qt5.15.2-mingw8.1.0-x86_64.zip "%CURL%" -L -o qwt6.2.0-qt5.15.2-mingw8.1.0-x86_64.zip https://github.com/mugiseyebrows/qwt-builds/releases/download/qwt6.2.0-qt5.15.2/qwt6.2.0-qt5.15.2-mingw8.1.0-x86_64.zip
"%P7Z%" x -y -oC:\ qwt6.2.0-qt5.15.2-mingw8.1.0-x86_64.zip
:qwt_end
if exist C:\mysql-8.2.0-winx64\bin\mysql.exe goto mysql_end
if not exist mysql-8.2.0-winx64.zip "%CURL%" -L -o mysql-8.2.0-winx64.zip https://cdn.mysql.com/Downloads/MySQL-8.2/mysql-8.2.0-winx64.zip
"%P7Z%" x -y -oC:\ mysql-8.2.0-winx64.zip
:mysql_end
qmake
mingw32-make release -j4
mugideploy collect --app mugi-query --git-version --bin release\mugi-query.exe --plugins odbc mysql sqlite psql --zip