# mugi-query

Sql client for mysql, sqlite, postgresql, access and mssql databases 

## Features

- Sessions
- Autocompletion
- Query history
- Join helper
- Data analysis tools: xy plot, distribution plot
- Query result to sql expression converter

## Screenshots

Main window

<img src="https://mugiseyebrows.github.io/img/mugi-query-plot.png" width="600"/>

Join helper

<img src="https://mugiseyebrows.github.io/img/mugi-query-join-helper.png" width="600"/>

Data import

<img src="https://mugiseyebrows.github.io/img/mugi-query-import.png" width="600"/>

## Build 

### Windows

1) Clone sources (in git bash run `git clone https://github.com/mugiseyebrows/mugi-query.git`)

2) Install `MSVC 2019` compiler (Download [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) and check `MSVC vxxx for VS 2019 for x64 or x86` and `SDK for Windows 10` in installer's components tab).

3) Install [qt5](https://d13lb3tujbc8s0.cloudfront.net/onlineinstallers/qt-unified-windows-x86-4.0.0-online.exe) from [www.qt.io](https://www.qt.io/download) (you need qt account for that, sorry) (check latest `Qt\Qt 5.x.x\MSVC 2019 64-bit` and leave `Developer and Designer tools` as it is (`Qt Creator, Debugging tools, cmake, ninja` checked))

4) Download qwt from [sourceforge.net](https://sourceforge.net/projects/qwt/files/qwt/6.1.4/qwt-6.1.4.zip/download)

5) Unzip `qwt-6.1.4.zip` to some temporary directory

6) Apply patch (in git bash navigate to qwt-6.1.4 parent directory and run `patch -p1 -ruN -d qwt-6.1.4 < path\to\mugi-query\src\qwt-6.1.4-msvc2019.patch`)

7) Build and install qwt using guide in `qwt-6.1.4/doc/html/qwtinstall.html`

8) Open `mugi-query.pro` in QtCreator and build project (assuming you have toolchain configured `Tools -> Optioins -> Kits`).

### Linux (Ubuntu)

```bash
# install qt6 and build tools
sudo apt install git build-essential cmake ninja-build qt6-base-dev libqt6svg6-dev libxkbcommon-dev

# build qwt
git clone https://git.code.sf.net/p/qwt/git qwt-git
cd qwt-git
qmake6
make
sudo install
cd ..

# install sql plugins
sudo apt install libqt6sql6-ibase libqt6sql6-mysql libqt6sql6-odbc libqt6sql6-psql libqt6sql6-sqlite

git clone https://github.com/mugiseyebrows/mugi-query.git
cd mugi-query
mkdir build
cd build
cmake -G Ninja ..
cmake --build .
sudo cmake --install .
cd ..
python3 desktop.py
```