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

![mugi-query-plot](https://mugiseyebrows.github.io/img/mugi-query-plot.png)

Join helper

![mugi-query-join-helper](https://mugiseyebrows.github.io/img/mugi-query-join-helper.png)

Data import

![mugi-query-import.png](https://mugiseyebrows.github.io/img/mugi-query-import.png)

## Build 

### Windows

1) Install qt5 from [www.qt.io](https://www.qt.io/download) ([direct link](http://master.qt.io/archive/qt/5.11/5.11.3/qt-opensource-windows-x86-5.11.3.exe))
2) Download qwt from [sourceforge.net](https://sourceforge.net/projects/qwt/files/qwt/6.1.4/qwt-6.1.4.zip/download)
3) Build and install qwt using guide in `doc/html/qwtinstall.html`
4) Include compiler and qt path to `%PATH%` env variable
5) In cmd run

```bash
git clone https://github.com/mugiseyebrows/mugi-query.git
cd mugi-query
qmake
make release
```

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