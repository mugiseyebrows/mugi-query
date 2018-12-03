#include <QApplication>
#include "mainwindow.h"

#include "queryparser.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //QueryParser::testSplit();

    return a.exec();
}
