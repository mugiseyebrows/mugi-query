#include "settings.h"

Settings* Settings::mInstance = 0;

#include <QDir>
#include <QApplication>
#include <QStandardPaths>
#include <QMessageBox>

Settings::Settings()
{

#if QT_VERSION >= 0x050000
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#else
    QString appData = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

    QString name = qApp->applicationName();
    QDir d(appData);
    if (!d.exists()) {
        d.cdUp();
        if (!d.mkdir(name)) {
            QString error = QString("Can not create directory %1").arg(appData);
            QMessageBox::critical(qApp->activeWindow(),"Error",error);
        }
        d.cd(name);
    }
    mDir = appData;
}

Settings *Settings::instance()
{
    if (!mInstance) {
        mInstance = new Settings();
    }
    return mInstance;
}

Settings::DateTimeFormat Settings::dateTimeFormat() const
{
    return mDateTimeFormat;
}

void Settings::setDateTimeFormat(Settings::DateTimeFormat format)
{
    mDateTimeFormat = format;
}

QString Settings::dir() const
{
    return mDir;
}
