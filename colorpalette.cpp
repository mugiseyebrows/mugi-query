#include "colorpalette.h"

ColorPalette *ColorPalette::mInstance = 0;

ColorPalette *ColorPalette::instance()
{
    if (!mInstance) {
        mInstance = new ColorPalette();
    }
    return mInstance;
}

ColorPalette::ColorPalette()
{
    colors["black"] = Qt::black;
    colors["white"] = Qt::white;
    colors["darkGray"] = Qt::darkGray;
    colors["gray"] = Qt::gray;
    colors["lightGray"] = Qt::lightGray;
    colors["red"] = Qt::red;
    colors["green"] = Qt::green;
    colors["blue"] = Qt::blue;
    colors["cyan"] = Qt::cyan;
    colors["magenta"] = Qt::magenta;
    colors["yellow"] = Qt::yellow;
    colors["darkRed"] = Qt::darkRed;
    colors["darkGreen"] = Qt::darkGreen;
    colors["darkBlue"] = Qt::darkBlue;
    colors["darkCyan"] = Qt::darkCyan;
    colors["darkMagenta"] = Qt::darkMagenta;
    colors["darkYellow"] = Qt::darkYellow;
    colors["transparent"] = Qt::transparent;
    colors["none"] = Qt::transparent;

    palette << "red"
            << "green"
            << "blue"
            << "cyan"
            << "magenta"
            << "yellow"
            << "darkRed"
            << "darkGreen"
            << "darkBlue"
            << "darkCyan"
            << "darkMagenta"
            << "darkYellow";
}

Qt::GlobalColor ColorPalette::toGlobalColor(const QString &color) const
{
    return colors.value(color,Qt::transparent);
}

QStringList ColorPalette::names() const
{
    return colors.keys();
}

QString ColorPalette::color(int row)
{
    return palette[row % palette.size()];
}
