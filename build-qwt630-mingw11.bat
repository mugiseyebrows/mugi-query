@echo off
rem This file is generated from build-qwt630-mingw11.pbat, all edits will be lost
set PATH=C:\mingw1120_64\bin;C:\Qt\6.7.1\mingw1120_64;C:\Program Files\Git\cmd;%PATH%
if exist "C:\Program Files\Git\usr\bin\patch.exe" set PATCH=C:\Program Files\Git\usr\bin\patch.exe
if not defined PATCH (
echo PATCH not found
exit /b
)
if exist "C:\Program Files\Git\mingw32\bin\curl.exe" set CURL=C:\Program Files\Git\mingw32\bin\curl.exe
if exist "C:\Program Files\Git\mingw64\bin\curl.exe" set CURL=C:\Program Files\Git\mingw64\bin\curl.exe
if exist "C:\Windows\System32\curl.exe" set CURL=C:\Windows\System32\curl.exe
if not defined CURL (
echo CURL not found
exit /b
)
if exist C:\qwt\6.3.0\mingw1120_64\lib\qwt.dll goto qwt630_end
if not exist qwt (
    git clone https://git.code.sf.net/p/qwt/git qwt
    pushd qwt
        git checkout v6.3.0
    popd
)
if not exist 0001-qwt-mingw1120_64.patch "%CURL%" -L -o 0001-qwt-mingw1120_64.patch https://gist.githubusercontent.com/mugiseyebrows/b9521434ce1b8aa798615a7b0541db58/raw/9464118e04a4cb82e522a0575f2581deff8f94c6/0001-qwt-mingw1120_64.patch
if not exist 0002-qwt-no-examples.patch "%CURL%" -L -o 0002-qwt-no-examples.patch https://gist.githubusercontent.com/mugiseyebrows/a04100f99b8f1f8c6815c3903aefad96/raw/a3b9e3087d0889a882f6129a3e07d11c06d1101f/0002-qwt-no-examples.patch
if not exist 0003-qwt-release.patch "%CURL%" -L -o 0003-qwt-release.patch https://gist.githubusercontent.com/mugiseyebrows/885a600fd06902254e224ba757446e25/raw/81ea4ae58764a511c60cdaeb81dbc9e9524220d4/0003-qwt-release.patch
pushd qwt
    "%PATCH%" -N -i ..\0001-qwt-mingw1120_64.patch
    "%PATCH%" -N -i ..\0002-qwt-no-examples.patch
    "%PATCH%" -N -i ..\0003-qwt-release.patch
    qmake
    mingw32-make -j4
    mingw32-make install
popd
:qwt630_end