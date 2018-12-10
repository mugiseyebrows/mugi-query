#include <QApplication>
#include "mainwindow.h"

#include "queryparser.h"
#include "version.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName(APP_NAME);
    a.setApplicationVersion(APP_VERSION);

    MainWindow w;
    w.show();

    //QueryParser::testSplit();

    return a.exec();
}
