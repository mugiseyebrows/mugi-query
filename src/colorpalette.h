#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <qnamespace.h>
#include <QStringList>
#include <QMap>
#include <QColor>

class ColorPalette
{
public:

    static ColorPalette* mInstance;
    static ColorPalette* instance();

    QColor toColor(const QString& name) const;

    QStringList names() const;

    QString color(int row);

    bool isTransparent(const QString &name) const;
protected:

    ColorPalette();

    QStringList colors;
    QStringList palette;

};

#endif // COLORPALETTE_H
