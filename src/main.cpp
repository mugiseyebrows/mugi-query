#include <QApplication>
#include "mainwindow.h"

#include "queryparser.h"
#include "version.h"
#include "tests.h"
#include "tokens.h"
#include "sqldatatypes.h"
#include "datetime.h"
#include <QDebug>
#include <QTimeZone>

#include <QFile>

int main(int argc, char *argv[])
{
    qRegisterMetaType<Tokens>();

    QApplication a(argc, argv);

    a.setApplicationName(APP_NAME);
    a.setApplicationVersion(APP_VERSION);

    MainWindow w;
    w.show();

    Tests::run();

    //DateTime::writeSamples();
    //DateTime::writeNumber();

    return a.exec();
}
