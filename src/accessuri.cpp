#include "accessuri.h"

QString accessUri(const QString& path) {

#ifdef _WIN64
    return QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=%1").arg(path);
#else
    return QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1").arg(path);
#endif

}
