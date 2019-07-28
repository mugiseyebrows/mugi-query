#include <QApplication>
#include "mainwindow.h"

#include "queryparser.h"
#include "version.h"
#include "tests.h"
#include "tokens.h"
#include "sqldatatypes.h"

int main(int argc, char *argv[])
{
    qRegisterMetaType<Tokens>();

    QApplication a(argc, argv);

    a.setApplicationName(APP_NAME);
    a.setApplicationVersion(APP_VERSION);

    MainWindow w;
    w.show();

    //Tests::run();
    SqlDataTypes::tryConvertTest();
    //SqlDataTypes::writeDateTimeSamples();

    return a.exec();
}
