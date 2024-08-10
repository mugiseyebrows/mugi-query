#ifndef EMMET_H
#define EMMET_H

#include <QString>

class Emmet
{
public:
    static QString parse(const QString &text, QString& error);
};

#endif // EMMET_H
