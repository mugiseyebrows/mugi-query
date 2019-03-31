#ifndef FORMATS_H
#define FORMATS_H

#include "dataformat.h"

#include <QString>

class Formats {
public:
    Formats(DataFormat::ActionType action);

    bool dateTimeUseLocale;
    QString dateFormat;
    QString timeFormat;
    bool realUseLocale;
};


#endif // FORMATS_H
