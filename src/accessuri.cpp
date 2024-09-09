#include "accessuri.h"
#include <QFileInfo>
#include <QDir>

QString accessUri(const QString& path) {

#ifdef _WIN64
    return QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; DBQ=%1").arg(QDir::toNativeSeparators(path));
#else
    return QString("DRIVER={Microsoft Access Driver (*.mdb)}; FIL={MS Access}; DBQ=%1").arg(QDir::toNativeSeparators(path));
#endif

}

QString excelUri(const QString& path) {
    return QString("DRIVER={Microsoft Excel Driver (*.xls, *.xlsx, *.xlsm, *.xlsb)}; DBQ=%1").arg(QDir::toNativeSeparators(path));
}

QString csvUri(const QString& path) {
    QFileInfo info(path);
    QString path_ = info.dir().absolutePath();
    return QString("DRIVER={Microsoft Access Text Driver (*.txt, *.csv)}; DBQ=%1").arg(QDir::toNativeSeparators(path_));
}
