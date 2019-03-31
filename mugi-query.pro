#-------------------------------------------------
#
# Project created by QtCreator 2018-08-12T12:16:47
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32 {

CONFIG(debug, debug|release) {
    LIBS += -LC:/Qwt-6.1.4/lib -lqwtd
} else {
    LIBS += -LC:/Qwt-6.1.4/lib -lqwt
}

INCLUDEPATH += C:/Qwt-6.1.4/include

}

unix {
    CONFIG += qwt
}

TARGET = mugi-query
TEMPLATE = app

INCLUDEPATH += src src/model src/widget

RC_ICONS += mugi-query.ico

DISTFILES += \
    todo.txt

target.path = /usr/local/bin
INSTALLS += target

HEADERS += \
    src/action.h \
    src/automation.h \
    src/colorpalette.h \
    src/copyeventfilter.h \
    src/dataformat.h \
    src/datastreamer.h \
    src/datautils.h \
    src/deleteeventfilter.h \
    src/filterplotitem.h \
    src/formats.h \
    src/highlighter.h \
    src/history.h \
    src/itemdelegate.h \
    src/itemdelegatewithcompleter.h \
    src/jointoken.h \
    src/jointokenlist.h \
    src/jsonhelper.h \
    src/lit.h \
    src/modelappender.h \
    src/outputtype.h \
    src/qisnumerictype.h \
    src/queryparser.h \
    src/relation.h \
    src/relations.h \
    src/rowvaluegetter.h \
    src/rowvaluesetter.h \
    src/sessionitem.h \
    src/setdefaultcolors.h \
    src/setheaderdata.h \
    src/settings.h \
    src/splitterutil.h \
    src/stringstringmap.h \
    src/tests.h \
    src/tokens.h \
    src/version.h \
    src/zipunzip.h \
    src/model/checkablestringlistmodel.h \
    src/model/distributionplotitem.h \
    src/model/distributionplotmodel.h \
    src/model/queriesstatmodel.h \
    src/model/relationsmodel.h \
    src/model/sessionmodel.h \
    src/model/stringlistmodelwithheader.h \
    src/model/xyplotmodel.h \
    src/model/xyplotmodelitem.h \
    src/widget/adddatabasedialog.h \
    src/widget/databasehistorydialog.h \
    src/widget/distributionplot.h \
    src/widget/intlineedit.h \
    src/widget/joinhelperwidget.h \
    src/widget/joinhelperwidgets.h \
    src/widget/mainwindow.h \
    src/widget/queryhistorywidget.h \
    src/widget/querymodelview.h \
    src/widget/removedatabasedialog.h \
    src/widget/savedatadialog.h \
    src/widget/sessiontab.h \
    src/widget/settingsdialog.h \
    src/widget/statview.h \
    src/widget/textedit.h \
    src/widget/userhelperdialog.h \
    src/widget/xyplot.h \
    src/plotpicker.h

SOURCES += \
    src/action.cpp \
    src/automation.cpp \
    src/colorpalette.cpp \
    src/copyeventfilter.cpp \
    src/dataformat.cpp \
    src/datastreamer.cpp \
    src/datautils.cpp \
    src/deleteeventfilter.cpp \
    src/filterplotitem.cpp \
    src/formats.cpp \
    src/highlighter.cpp \
    src/history.cpp \
    src/itemdelegate.cpp \
    src/itemdelegatewithcompleter.cpp \
    src/jointokenlist.cpp \
    src/jsonhelper.cpp \
    src/lit.cpp \
    src/main.cpp \
    src/modelappender.cpp \
    src/outputtype.cpp \
    src/qisnumerictype.cpp \
    src/queryparser.cpp \
    src/relation.cpp \
    src/relations.cpp \
    src/rowvaluegetter.cpp \
    src/rowvaluesetter.cpp \
    src/sessionitem.cpp \
    src/setdefaultcolors.cpp \
    src/setheaderdata.cpp \
    src/settings.cpp \
    src/splitterutil.cpp \
    src/stringstringmap.cpp \
    src/tests.cpp \
    src/tokens.cpp \
    src/model/checkablestringlistmodel.cpp \
    src/model/distributionplotitem.cpp \
    src/model/distributionplotmodel.cpp \
    src/model/queriesstatmodel.cpp \
    src/model/relationsmodel.cpp \
    src/model/sessionmodel.cpp \
    src/model/stringlistmodelwithheader.cpp \
    src/model/xyplotmodel.cpp \
    src/model/xyplotmodelitem.cpp \
    src/widget/adddatabasedialog.cpp \
    src/widget/databasehistorydialog.cpp \
    src/widget/distributionplot.cpp \
    src/widget/intlineedit.cpp \
    src/widget/joinhelperwidget.cpp \
    src/widget/joinhelperwidgets.cpp \
    src/widget/mainwindow.cpp \
    src/widget/queryhistorywidget.cpp \
    src/widget/querymodelview.cpp \
    src/widget/removedatabasedialog.cpp \
    src/widget/savedatadialog.cpp \
    src/widget/sessiontab.cpp \
    src/widget/settingsdialog.cpp \
    src/widget/statview.cpp \
    src/widget/textedit.cpp \
    src/widget/userhelperdialog.cpp \
    src/widget/xyplot.cpp \
    src/plotpicker.cpp

FORMS += \
    src/widget/adddatabasedialog.ui \
    src/widget/databasehistorydialog.ui \
    src/widget/distributionplot.ui \
    src/widget/joinhelperwidget.ui \
    src/widget/joinhelperwidgets.ui \
    src/widget/mainwindow.ui \
    src/widget/queryhistorywidget.ui \
    src/widget/querymodelview.ui \
    src/widget/removedatabasedialog.ui \
    src/widget/savedatadialog.ui \
    src/widget/sessiontab.ui \
    src/widget/settingsdialog.ui \
    src/widget/userhelperdialog.ui \
    src/widget/xyplot.ui

