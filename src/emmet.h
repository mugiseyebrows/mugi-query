#ifndef EMMET_H
#define EMMET_H

#include "emmet_global.h"

#include <QString>

class EMMET_EXPORT Emmet
{
public:
    static QString parse(const QString &text, QString& error);
};

#endif // EMMET_H
