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
    savedatadialog.cpp \
    checkablestringlistmodel.cpp \
    datastreamer.cpp \
    dataformat.cpp \
    queriesstatmodel.cpp \
    setsplitersizesratio.cpp \
    outputtype.cpp \
    copyeventfilter.cpp \
    itemdelegate.cpp \
    settings.cpp \
    tokens.cpp \
    textedit.cpp \
    highlighter.cpp \
    userhelperdialog.cpp \
    modelappender.cpp \
    itemdelegatewithcompleter.cpp \
    relation.cpp \
    relations.cpp \
    relationsmodel.cpp \
    joinhelperwidget.cpp \
    jsonhelper.cpp \
    stringlistmodelwithheader.cpp \
    settingsdialog.cpp \
    displayformat.cpp

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
    savedatadialog.h \
    checkablestringlistmodel.h \
    datastreamer.h \
    dataformat.h \
    queriesstatmodel.h \
    setsplitersizesratio.h \
    outputtype.h \
    copyeventfilter.h \
    itemdelegate.h \
    settings.h \
    tokens.h \
    textedit.h \
    highlighter.h \
    userhelperdialog.h \
    version.h \
    modelappender.h \
    itemdelegatewithcompleter.h \
    relation.h \
    relations.h \
    relationsmodel.h \
    joinhelperwidget.h \
    jsonhelper.h \
    stringlistmodelwithheader.h \
    settingsdialog.h \
    displayformat.h

FORMS    += mainwindow.ui \
    sessiontab.ui \
    adddatabasedialog.ui \
    removedatabasedialog.ui \
    databasehistorydialog.ui \
    queryhistorywidget.ui \
    savedatadialog.ui \
    userhelperdialog.ui \
    joinhelperwidget.ui \
    settingsdialog.ui

RC_ICONS += mugi-query.ico

DISTFILES += \
    todo.txt

