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

In terminal run

```bash
# installs qt5 and build tools
sudo apt install git build-essential qtbase5-dev qtchooser

# installs qwt and dependencies
sudo apt install libqwt-qt5-dev libqt5svg5-dev 

# installs sql plugins
sudo apt install libqt5sql5-mysql libqt5sql5-odbc libqt5sql5-psql libqt5sql5-sqlite libqt5sql5-tds libqt5sql5-ibase 

git clone https://github.com/mugiseyebrows/mugi-query.git
cd mugi-query
qmake -qt=5 'CONFIG+=release'
make
make clean
strip mugi-query
sudo make install # installs to /usr/local/bin
python desktop.py # copies icons, creates desktop file
```