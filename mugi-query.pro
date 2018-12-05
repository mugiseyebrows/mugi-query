#-------------------------------------------------
#
# Project created by QtCreator 2018-08-12T12:16:47
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mugi-query
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sessionmodel.cpp \
    sessionitem.cpp \
    sessiontab.cpp \
    adddatabasedialog.cpp \
    removedatabasedialog.cpp \
    databasehistorydialog.cpp \
    history.cpp \
    queryparser.cpp \
    sl.cpp \
    rowvaluegetter.cpp \
    rowvaluesetter.cpp \
    queryhistorywidget.cpp \
    schemafetcher.cpp \
    plaintextedit.cpp \
    savedatadialog.cpp \
    checkablestringlistmodel.cpp \
    datastreamer.cpp \
    dataformat.cpp \
    queriesstatmodel.cpp \
    setsplitersizesratio.cpp \
    outputtype.cpp

HEADERS  += mainwindow.h \
    sessionmodel.h \
    sessionitem.h \
    sessiontab.h \
    adddatabasedialog.h \
    removedatabasedialog.h \
    databasehistorydialog.h \
    history.h \
    queryparser.h \
    sl.h \
    rowvaluegetter.h \
    rowvaluesetter.h \
    queryhistorywidget.h \
    schemafetcher.h \
    plaintextedit.h \
    savedatadialog.h \
    checkablestringlistmodel.h \
    datastreamer.h \
    dataformat.h \
    queriesstatmodel.h \
    setsplitersizesratio.h \
    outputtype.h

FORMS    += mainwindow.ui \
    sessiontab.ui \
    adddatabasedialog.ui \
    removedatabasedialog.ui \
    databasehistorydialog.ui \
    queryhistorywidget.ui \
    savedatadialog.ui
