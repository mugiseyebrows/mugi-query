#-------------------------------------------------
#
# Project created by QtCreator 2018-08-12T12:16:47
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

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

include(src/richheaderview/richheaderview.pri)
include(src/tablebuttons/tablebuttons.pri)

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
    src/widget/databasehistorydialog.h \
    src/widget/distributionplot.h \
    src/widget/intlineedit.h \
    src/widget/joinhelperwidget.h \
    src/widget/joinhelperwidgets.h \
    src/widget/mainwindow.h \
    src/widget/queryhistorywidget.h \
    src/widget/querymodelview.h \
    src/widget/savedatadialog.h \
    src/widget/sessiontab.h \
    src/widget/settingsdialog.h \
    src/widget/statview.h \
    src/widget/textedit.h \
    src/widget/userhelperdialog.h \
    src/widget/xyplot.h \
    src/plotpicker.h \
    src/canvaspicker.h \
    src/plotmultibarchart.h \
    src/clipboard.h \
    src/error.h \
    src/widget/databaseconnectdialog.h \
    src/model/schemamodel.h \
    src/model/schemaitem.h \
    src/widget/doublelineedit.h \
    src/widget/dirstibutionplotoptionsedit.h \
    src/histogram.h \
    src/distributiondataset.h \
    src/model/dataimportmodel.h \
    src/widget/dataimportwidgets.h \
    src/widget/dataimportwidget.h \
    src/sqldatatypes.h \
    src/model/dataimportcolumnmodel.h \
    src/callonce.h \
    src/widget/selectcolumnswidget.h \
    src/widget/selectcolumnslistwidget.h \
    src/datetime.h \
    src/testdatetimesample.h \
    src/testdatesample.h \
    src/timezone.h \
    src/multinameenum.h \
    src/widget/fieldattributeswidget.h \
    src/field.h \
    src/testdatetime2sample.h \
    src/datasavedialogstate.h \
    src/timezones.h \
    src/drivernames.h \
    src/widget/datetimerangewidget.h \
    src/datetimerangewidgetmanager.h \
    src/model/queryhistorymodel.h \
    src/mugisql/mugisql.h \
    src/widget/selectcolumnsdialog.h \
    src/model/distributionhistogrammodel.h \
    src/doubleitemdelegate.h \
    src/widget/clipboardutil.h

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
    src/widget/databasehistorydialog.cpp \
    src/widget/distributionplot.cpp \
    src/widget/intlineedit.cpp \
    src/widget/joinhelperwidget.cpp \
    src/widget/joinhelperwidgets.cpp \
    src/widget/mainwindow.cpp \
    src/widget/queryhistorywidget.cpp \
    src/widget/querymodelview.cpp \
    src/widget/savedatadialog.cpp \
    src/widget/sessiontab.cpp \
    src/widget/settingsdialog.cpp \
    src/widget/statview.cpp \
    src/widget/textedit.cpp \
    src/widget/userhelperdialog.cpp \
    src/widget/xyplot.cpp \
    src/plotpicker.cpp \
    src/canvaspicker.cpp \
    src/plotmultibarchart.cpp \
    src/clipboard.cpp \
    src/error.cpp \
    src/widget/databaseconnectdialog.cpp \
    src/model/schemamodel.cpp \
    src/model/schemaitem.cpp \
    src/widget/doublelineedit.cpp \
    src/widget/dirstibutionplotoptionsedit.cpp \
    src/histogram.cpp \
    src/distributiondataset.cpp \
    src/model/dataimportmodel.cpp \
    src/widget/dataimportwidgets.cpp \
    src/widget/dataimportwidget.cpp \
    src/sqldatatypes.cpp \
    src/model/dataimportcolumnmodel.cpp \
    src/callonce.cpp \
    src/widget/selectcolumnswidget.cpp \
    src/widget/selectcolumnslistwidget.cpp \
    src/datetime.cpp \
    src/testdatetimesample.cpp \
    src/testdatesample.cpp \
    src/timezone.cpp \
    src/multinameenum.cpp \
    src/widget/fieldattributeswidget.cpp \
    src/field.cpp \
    src/testdatetime2sample.cpp \
    src/datasavedialogstate.cpp \
    src/timezones.cpp \
    src/widget/datetimerangewidget.cpp \
    src/datetimerangewidgetmanager.cpp \
    src/model/queryhistorymodel.cpp \
    src/mugisql/mugisql.cpp \
    src/widget/selectcolumnsdialog.cpp \
    src/model/distributionhistogrammodel.cpp \
    src/doubleitemdelegate.cpp \
    src/widget/clipboardutil.cpp

FORMS += \
    src/widget/databasehistorydialog.ui \
    src/widget/distributionplot.ui \
    src/widget/joinhelperwidget.ui \
    src/widget/joinhelperwidgets.ui \
    src/widget/mainwindow.ui \
    src/widget/queryhistorywidget.ui \
    src/widget/querymodelview.ui \
    src/widget/savedatadialog.ui \
    src/widget/sessiontab.ui \
    src/widget/settingsdialog.ui \
    src/widget/userhelperdialog.ui \
    src/widget/xyplot.ui \
    src/widget/databaseconnectdialog.ui \
    src/widget/dirstibutionplotoptionsedit.ui \
    src/widget/dataimportwidgets.ui \
    src/widget/dataimportwidget.ui \
    src/widget/selectcolumnswidget.ui \
    src/widget/selectcolumnslistwidget.ui \
    src/widget/fieldattributeswidget.ui \
    src/widget/datetimerangewidget.ui \
    src/widget/selectcolumnsdialog.ui

RESOURCES += \
    icons.qrc

