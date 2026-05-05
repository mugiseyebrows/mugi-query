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

```bash
git clone https://github.com/mugiseyebrows/mugi-query.git
cd mugi-query
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -B Release -DCMAKE_PREFIX_PATH=C:/Qwt-6.3.1-Qt-6.10.0
cmake --build Release
```

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
# build
git clone https://github.com/mugiseyebrows/mugi-query.git
cd mugi-query
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -B Release
cmake --build Release
# install
sudo cmake --install Release
python3 desktop.py
```