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
    history.cpp

HEADERS  += mainwindow.h \
    sessionmodel.h \
    sessionitem.h \
    sessiontab.h \
    adddatabasedialog.h \
    removedatabasedialog.h \
    databasehistorydialog.h \
    history.h

FORMS    += mainwindow.ui \
    sessiontab.ui \
    adddatabasedialog.ui \
    removedatabasedialog.ui \
    databasehistorydialog.ui
