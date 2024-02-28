#include <QApplication>
#include "mainwindow.h"

#include "version.h"
#include "tests.h"
#include "datetime.h"
#include <QDebug>
#include <QTimeZone>

int main(int argc, char *argv[])
{
    qRegisterMetaType<Tokens>();

    QApplication a(argc, argv);

    a.setApplicationName("mugi-query");
    a.setApplicationVersion(VERSION);

    MainWindow w;
    w.show();

    //Tests::run();

    //DateTime::writeSamples();
    //DateTime::writeNumber();
    //DateTime::writeTimeZones();

    return a.exec();
}
