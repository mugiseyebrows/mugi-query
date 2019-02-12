#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <qnamespace.h>
#include <QStringList>
#include <QMap>

class ColorPalette
{
public:

    static ColorPalette* mInstance;
    static ColorPalette* instance();

    Qt::GlobalColor toGlobalColor(const QString& color) const;

    QStringList names() const;

    QString color(int row);

protected:

    ColorPalette();

    QMap<QString, Qt::GlobalColor> colors;
    QStringList palette;

};

#endif // COLORPALETTE_H
