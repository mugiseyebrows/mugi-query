# Generated from mugi-query.pro.

cmake_minimum_required(VERSION 3.16)
project(mugi-query VERSION 1.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(BUILD_APP "" ON)

if (WIN32)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql Svg Widgets Test AxContainer)
else()
find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql Svg Widgets Test)
endif()

find_package(Qwt REQUIRED)

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTOUIC TRUE)

qt_add_executable(tst_emmet
    src/tst_emmet.cpp
    src/emmet.cpp
    src/emmet.h
)
add_test(NAME tst_emmet COMMAND tst_emmet)
target_link_libraries(tst_emmet PRIVATE Qt::Test)
target_include_directories(tst_emmet PRIVATE src)

qt_add_executable(tst_sqlparse
    src/sqlparse.cpp
    src/sqlparse.h
    src/tst_sqlparse.cpp
)
add_test(NAME tst_sqlparse COMMAND tst_sqlparse)
target_link_libraries(tst_sqlparse PRIVATE Qt::Test)
target_include_directories(tst_sqlparse PRIVATE src)

qt_add_executable(tst_sdata src/schema2/sdata.h src/schema2/sdata.cpp src/tst_sdata.cpp)
add_test(NAME tst_sdata COMMAND tst_sdata)
target_link_libraries(tst_sdata PRIVATE Qt::Test)
target_include_directories(tst_sdata PRIVATE src src/schema2)

set(icons_resource_files
    "src/icons/9022095_arrows_out_cardinal_duotone_icon.png"
    "src/icons/9022100_browser_duotone_icon.png"
    "src/icons/9022231_download_duotone_icon.png"
    "src/icons/9022355_download_simple_duotone_icon.png"
    "src/icons/9022360_file_code_duotone_icon.png"
    "src/icons/9022583_magnifying_glass_plus_duotone_icon.png"
    "src/icons/9022683_image_duotone_icon.png"
    "src/icons/9022691_magnifying_glass_minus_duotone_icon.png"
    "src/icons/9022692_magnifying_glass_duotone_icon.png"
    "src/icons/9022711_pencil_duotone_icon.png"
    "src/icons/9022743_squares_four_duotone_icon.png"
    "src/icons/9022808_plugs_connected_duotone_icon.png"
    "src/icons/9022809_plugs_duotone_icon.png"
    "src/icons/9022810_plus_duotone_icon.png"
    "src/icons/9022869_trash_duotone_icon.png"
    "src/icons/9022926_upload_duotone_icon.png"
    "src/icons/9022961_table_duotone_icon.png"
    "src/icons/undo.png"
)

# cmake -D BUILD_APP=ON ..
# cmake -D BUILD_APP=OFF ..

if (BUILD_APP)
    qt_add_executable(mugi-query WIN32
        src/accessuri.cpp src/accessuri.h
        src/action.cpp src/action.h
        src/automate.cpp src/automate.h
        src/automation.cpp src/automation.h
        src/callonce.cpp src/callonce.h
        src/choicedialog.cpp src/choicedialog.h src/choicedialog.ui
        src/clicklistener.cpp src/clicklistener.h
        src/clipboard.cpp src/clipboard.h
        src/colorpalette.cpp src/colorpalette.h
        src/completer.cpp src/completer.h
        src/completerdata.cpp src/completerdata.h
        src/confirmationdialog.cpp src/confirmationdialog.h src/confirmationdialog.ui
        src/copyeventfilter.cpp src/copyeventfilter.h
        src/dataformat.cpp src/dataformat.h
        src/datasavedialogstate.cpp src/datasavedialogstate.h
        src/datastreamer.cpp src/datastreamer.h
        src/datautils.cpp src/datautils.h
        src/datetime.cpp src/datetime.h
        src/datetimerangewidgetmanager.cpp src/datetimerangewidgetmanager.h
        src/deleteeventfilter.cpp src/deleteeventfilter.h
        src/distributiondataset.cpp src/distributiondataset.h
        src/doubleitemdelegate.cpp src/doubleitemdelegate.h
        src/drivernames.h
        src/enums.h
        src/error.cpp src/error.h
        src/field.cpp src/field.h
        src/fieldnames.cpp src/fieldnames.h
        src/filterempty.cpp src/filterempty.h
        src/filterplotitem.cpp src/filterplotitem.h
        src/formats.cpp src/formats.h
        src/hash.cpp src/hash.h
        src/highlighter.cpp src/highlighter.h
        src/histogram.cpp src/histogram.h
        src/history.cpp src/history.h
        src/itemdelegate.cpp src/itemdelegate.h
        src/itemdelegatewithcompleter.cpp src/itemdelegatewithcompleter.h
        src/jointoken.h
        src/jointokenlist.cpp src/jointokenlist.h
        src/jsonhelper.cpp src/jsonhelper.h
        src/main.cpp
        src/model/checkablemodel.cpp src/model/checkablemodel.h
        src/model/checkablestringlistmodel.cpp src/model/checkablestringlistmodel.h
        src/model/datacomparemodel.cpp src/model/datacomparemodel.h
        src/model/dataimportcolumnmodel.cpp src/model/dataimportcolumnmodel.h
        src/model/dataimportmodel.cpp src/model/dataimportmodel.h
        src/model/distributionhistogrammodel.cpp src/model/distributionhistogrammodel.h
        src/model/distributionplotitem.cpp src/model/distributionplotitem.h
        src/model/distributionplotmodel.cpp src/model/distributionplotmodel.h
        src/model/queriesstatmodel.cpp src/model/queriesstatmodel.h
        src/model/queryhistorymodel.cpp src/model/queryhistorymodel.h
        src/model/relationsmodel.cpp src/model/relationsmodel.h
        src/model/schemaitem.cpp src/model/schemaitem.h
        src/model/schemamodel.cpp src/model/schemamodel.h
        src/model/sessionmodel.cpp src/model/sessionmodel.h
        src/model/stringlistmodelwithheader.cpp src/model/stringlistmodelwithheader.h
        src/model/xjoinmodel.cpp src/model/xjoinmodel.h
        src/model/xyplotmodel.cpp src/model/xyplotmodel.h
        src/model/xyplotmodelitem.cpp src/model/xyplotmodelitem.h
        src/modelappender.cpp src/modelappender.h
        src/modelcolumn.cpp src/modelcolumn.h
        src/multinameenum.cpp src/multinameenum.h
        src/ones.cpp src/ones.h
        src/outputtype.cpp src/outputtype.h
        src/plotmultibarchart.cpp src/plotmultibarchart.h
        src/plotpicker.cpp src/plotpicker.h
        src/qisnumerictype.cpp src/qisnumerictype.h
        src/queryparser.cpp src/queryparser.h
        src/qwt_compat.cpp src/qwt_compat.h
        src/relation.cpp src/relation.h
        src/relations.cpp src/relations.h
        src/rowvaluegetter.cpp src/rowvaluegetter.h
        src/rowvaluesetter.cpp src/rowvaluesetter.h
        src/richheaderview/richheadercell.cpp src/richheaderview/richheadercell.h
        src/richheaderview/richheadercellbitmap.cpp src/richheaderview/richheadercellbitmap.h
        src/richheaderview/richheadercellimpl.cpp src/richheaderview/richheadercellimpl.h
        src/richheaderview/richheaderdata.cpp src/richheaderview/richheaderdata.h
        src/richheaderview/richheaderdataimpl.cpp src/richheaderview/richheaderdataimpl.h
        src/richheaderview/richheaderdataimplbase.cpp src/richheaderview/richheaderdataimplbase.h
        src/richheaderview/richheaderdirection.cpp src/richheaderview/richheaderdirection.h
        src/richheaderview/richheaderiterator.cpp src/richheaderview/richheaderiterator.h
        src/richheaderview/richheadertabfilter.cpp src/richheaderview/richheadertabfilter.h
        src/richheaderview/richheadertests.cpp src/richheaderview/richheadertests.h
        src/richheaderview/richheadertextfitter.cpp src/richheaderview/richheadertextfitter.h
        src/richheaderview/richheaderview.cpp src/richheaderview/richheaderview.h
        src/schema2/datacomparewidget2.cpp src/schema2/datacomparewidget2.h src/schema2/datacomparewidget2.ui
        src/schema2/datacomparewidgetitem.cpp src/schema2/datacomparewidgetitem.h src/schema2/datacomparewidgetitem.ui
        src/schema2/reporterror.cpp src/schema2/reporterror.h
        src/schema2/schema2actiontoolbar.cpp src/schema2/schema2actiontoolbar.h src/schema2/schema2actiontoolbar.ui
        src/schema2/schema2alterview.cpp src/schema2/schema2alterview.h src/schema2/schema2alterview.ui
        src/schema2/schema2arrange.cpp src/schema2/schema2arrange.h
        src/schema2/schema2changeset.cpp src/schema2/schema2changeset.h
        src/schema2/schema2data.cpp src/schema2/schema2data.h
        src/schema2/schema2export.cpp src/schema2/schema2export.h
        src/schema2/schema2index.cpp src/schema2/schema2index.h
        src/schema2/schema2indexesmodel.cpp src/schema2/schema2indexesmodel.h
        src/schema2/schema2join.cpp src/schema2/schema2join.h
        src/schema2/schema2parentrelationsmodel.cpp src/schema2/schema2parentrelationsmodel.h
        src/schema2/schema2pushview.cpp src/schema2/schema2pushview.h src/schema2/schema2pushview.ui
        src/schema2/schema2relatedtableswidget.cpp src/schema2/schema2relatedtableswidget.h src/schema2/schema2relatedtableswidget.ui
        src/schema2/schema2relation.cpp src/schema2/schema2relation.h
        src/schema2/schema2relationdialog2.cpp src/schema2/schema2relationdialog2.h src/schema2/schema2relationdialog2.ui
        src/schema2/schema2relationguesser.cpp src/schema2/schema2relationguesser.h
        src/schema2/schema2relationitem2.cpp src/schema2/schema2relationitem2.h
        src/schema2/schema2relationmodel.cpp src/schema2/schema2relationmodel.h
        src/schema2/schema2relationsfiltermodel.cpp src/schema2/schema2relationsfiltermodel.h
        src/schema2/schema2relationslistdialog.ui
        src/schema2/schema2relationsmodel.cpp src/schema2/schema2relationsmodel.h
        src/schema2/schema2status.cpp src/schema2/schema2status.h
        src/schema2/schema2store.cpp src/schema2/schema2store.h
        src/schema2/schema2synctoolbar.cpp src/schema2/schema2synctoolbar.h src/schema2/schema2synctoolbar.ui
        src/schema2/schema2tablecolumn.cpp src/schema2/schema2tablecolumn.h
        src/schema2/schema2tableitem.cpp src/schema2/schema2tableitem.h
        src/schema2/schema2tablemodel.cpp src/schema2/schema2tablemodel.h
        src/schema2/schema2tablesmodel.cpp src/schema2/schema2tablesmodel.h
        src/schema2/schema2toolbar.cpp src/schema2/schema2toolbar.h src/schema2/schema2toolbar.ui
        src/schema2/schema2treemodel.cpp src/schema2/schema2treemodel.h
        src/schema2/schema2treeproxymodel.cpp src/schema2/schema2treeproxymodel.h
        src/schema2/schema2view.cpp src/schema2/schema2view.h src/schema2/schema2view.ui
        src/schema2/schema2zoomtoolbar.cpp src/schema2/schema2zoomtoolbar.h src/schema2/schema2zoomtoolbar.ui
        src/schema2/sdata.cpp src/schema2/sdata.h
        src/schema2/setcompleter.cpp src/schema2/setcompleter.h
        src/schema2/sqlescaper.cpp src/schema2/sqlescaper.h
        src/schema2/tablestretcher.cpp src/schema2/tablestretcher.h
        src/schema2/uncheckedmode.cpp src/schema2/uncheckedmode.h
        src/sessionitem.cpp src/sessionitem.h
        src/setdefaultcolors.cpp src/setdefaultcolors.h
        src/setheaderdata.cpp src/setheaderdata.h
        src/settings.cpp src/settings.h
        src/showandraise.cpp src/showandraise.h
        src/showhidefilter.cpp src/showhidefilter.h
        src/splitterutil.cpp src/splitterutil.h
        src/sqldatatypes.cpp src/sqldatatypes.h
        src/sqlhelper.cpp src/sqlhelper.h
        src/sqlutil.cpp src/sqlutil.h
        src/tablebuttons/tablebutton.cpp src/tablebuttons/tablebutton.h
        src/tablebuttons/tablebuttongroup.cpp src/tablebuttons/tablebuttongroup.h
        src/tablebuttons/tablebuttonimpl.cpp src/tablebuttons/tablebuttonimpl.h
        src/tablebuttons/tablebuttons.cpp src/tablebuttons/tablebuttons.h
        src/testdatesample.cpp src/testdatesample.h
        src/testdatetime2sample.cpp src/testdatetime2sample.h
        src/testdatetimesample.cpp src/testdatetimesample.h
        src/tests.cpp src/tests.h
        src/timezone.cpp src/timezone.h
        src/timezones.cpp src/timezones.h
        src/tokens.cpp src/tokens.h
        src/tolower.cpp src/tolower.h
        src/tools.cpp src/tools.h
        src/version.h
        src/widget/checkableview.cpp src/widget/checkableview.h src/widget/checkableview.ui
        src/widget/clipboardutil.cpp src/widget/clipboardutil.h
        src/widget/comparetableitemwidget.cpp src/widget/comparetableitemwidget.h src/widget/comparetableitemwidget.ui
        src/widget/comparetablewidget.cpp src/widget/comparetablewidget.h src/widget/comparetablewidget.ui
        src/widget/databaseconnectdialog.cpp src/widget/databaseconnectdialog.h src/widget/databaseconnectdialog.ui
        src/widget/databasehistorydialog.cpp src/widget/databasehistorydialog.h src/widget/databasehistorydialog.ui
        src/widget/datacomparewidget.cpp src/widget/datacomparewidget.h src/widget/datacomparewidget.ui
        src/widget/dataimportwidget.cpp src/widget/dataimportwidget.h src/widget/dataimportwidget.ui
        src/widget/dataimportwidgets.cpp src/widget/dataimportwidgets.h src/widget/dataimportwidgets.ui
        src/widget/datatruncatedialog.cpp src/widget/datatruncatedialog.h src/widget/datatruncatedialog.ui
        src/widget/datetimerangewidget.cpp src/widget/datetimerangewidget.h src/widget/datetimerangewidget.ui
        src/widget/dirstibutionplotoptionsedit.cpp src/widget/dirstibutionplotoptionsedit.h src/widget/dirstibutionplotoptionsedit.ui
        src/widget/distributionplot.cpp src/widget/distributionplot.h src/widget/distributionplot.ui
        src/widget/doublelineedit.cpp src/widget/doublelineedit.h
        src/widget/exportdialog.cpp src/widget/exportdialog.h src/widget/exportdialog.ui
        src/widget/fieldattributeswidget.cpp src/widget/fieldattributeswidget.h src/widget/fieldattributeswidget.ui
        src/widget/filepathwidget.cpp src/widget/filepathwidget.h src/widget/filepathwidget.ui
        src/widget/intlineedit.cpp src/widget/intlineedit.h
        src/widget/joinhelperwidget.cpp src/widget/joinhelperwidget.h src/widget/joinhelperwidget.ui
        src/widget/joinhelperwidgets.cpp src/widget/joinhelperwidgets.h src/widget/joinhelperwidgets.ui
        src/widget/lineselect.cpp src/widget/lineselect.h src/widget/lineselect.ui
        src/widget/mainwindow.cpp src/widget/mainwindow.h src/widget/mainwindow.ui
        src/widget/queryhistorywidget.cpp src/widget/queryhistorywidget.h src/widget/queryhistorywidget.ui
        src/widget/querymodelview.cpp src/widget/querymodelview.h src/widget/querymodelview.ui
        src/widget/radiobuttons.cpp src/widget/radiobuttons.h src/widget/radiobuttons.ui
        src/widget/savedatadialog.cpp src/widget/savedatadialog.h src/widget/savedatadialog.ui
        src/widget/selectcolumnsdialog.cpp src/widget/selectcolumnsdialog.h src/widget/selectcolumnsdialog.ui
        src/widget/selectcolumnslistwidget.cpp src/widget/selectcolumnslistwidget.h src/widget/selectcolumnslistwidget.ui
        src/widget/selectcolumnswidget.cpp src/widget/selectcolumnswidget.h src/widget/selectcolumnswidget.ui
        src/widget/sessiontab.cpp src/widget/sessiontab.h src/widget/sessiontab.ui
        src/widget/settingsdialog.cpp src/widget/settingsdialog.h src/widget/settingsdialog.ui
        src/widget/statview.cpp src/widget/statview.h
        src/widget/textedit.cpp src/widget/textedit.h
        src/widget/toolmysqldialog.cpp src/widget/toolmysqldialog.h src/widget/toolmysqldialog.ui
        src/widget/toolmysqldumpdialog.cpp src/widget/toolmysqldumpdialog.h src/widget/toolmysqldumpdialog.ui
        src/widget/userhelperdialog.cpp src/widget/userhelperdialog.h src/widget/userhelperdialog.ui
        src/widget/xjoinitemwidget.cpp src/widget/xjoinitemwidget.h src/widget/xjoinitemwidget.ui
        src/widget/xjoinwidget.cpp src/widget/xjoinwidget.h src/widget/xjoinwidget.ui
        src/widget/xyplot.cpp src/widget/xyplot.h src/widget/xyplot.ui
        src/zipunzip.h src/query_exec.h
        src/widget/daterangewidget.h src/widget/daterangewidget.cpp src/widget/daterangewidget.ui
        src/widget/relatetoolwidget.h src/widget/relatetoolwidget.cpp src/widget/relatetoolwidget.ui
        src/sqlparse.cpp src/sqlparse.h
        src/emmet.cpp src/emmet.h
        src/schema2/dataimportwidget2.cpp src/schema2/dataimportwidget2.h src/schema2/dataimportwidget2.ui
        mugi-query_resource.rc
        src/model/hexitemdelegate.h src/model/hexitemdelegate.cpp
        src/widget/actionrunstepswidget.h src/widget/actionrunstepswidget.cpp src/widget/actionrunstepswidget.ui
        src/schema2/codewidget.h src/schema2/codewidget.cpp src/schema2/codewidget.ui
        src/schema2/graphicsview.cpp src/schema2/graphicsview.h
        src/schema2/style.h src/schema2/style.cpp
        src/schema2/stylewidget.h src/schema2/stylewidget.cpp src/schema2/stylewidget.ui
        src/schema2/odbcuri.h src/schema2/odbcuri.cpp
    )
    target_include_directories(mugi-query PUBLIC
        src
        src/model
        src/schema2
        src/tablebuttons
        src/widget
    )
    qt6_add_resources(mugi-query "icons"
        PREFIX "/icons"
        FILES ${icons_resource_files}
    )
    target_link_libraries(mugi-query PUBLIC
        Qt::Widgets
        Qt::Sql
        Qt::Svg
        Qwt
    )

    if (WIN32)
        target_link_libraries(mugi-query PUBLIC Qt::AxContainer)
    endif()

    # LD_LIBRARY_PATH="/usr/local/qwt-6.3.0-dev/lib:/usr/local/lib" /usr/local/bin/mugi-query
    install(TARGETS mugi-query)
    install(FILES emmet.json DESTINATION share)
endif()




