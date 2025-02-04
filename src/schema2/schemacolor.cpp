#include "schemacolor.h"

#include <QMap>
#include <QDebug>

/*static*/
SchemaColor* SchemaColor::mInstance = nullptr;

static int mapv(int v, int vmin, int vmax, int d) {

    // mapv 0 255 -> 0..360..10
    // nmax = (vmax - vmin) / d
    // n = (v % nmax) * d + vmin
    int nmax = (vmax - vmin) / d;
    int n = (v % nmax) * d + vmin;
    return n;
}

QColor SchemaColor::color(const QString &schema) {

    static QMap<QString, QColor> cache;
    if (!cache.contains(schema)) {
        size_t hash = qHash(schema, 7);
        uint8_t v1 = hash & 0xff;
        uint8_t v2 = (hash >> 8) & 0xff;
        uint8_t v3 = (hash >> 16) & 0xff;
        // hsv255 x 125-240 64-117
        int h = mapv(v1, 0, 359, 9);
        int s = mapv(v2, 127, 229, 10);
        int v = mapv(v3, 64, 117, 10);
        cache[schema] = QColor::fromHsv(h, s, v);
        qDebug() << "color(" << schema << ")=" << h << s << v;
    }
    return cache[schema];

#if 0
    if (!mColors.contains(schema)) {
        mColors[schema] = mIndex;
        mIndex = (mIndex + 1) % mPalette.size();
    }
    return mPalette[mColors[schema]];
#endif
}

SchemaColor::SchemaColor() {
    mPalette = {"#491704", "#1B4242", "#4D3C77", "#5D0E41"};
    // hsv100 16 94 28, 180 59 25, 257 49 46, 321 84 36
    // hsv100 x 49-94 25-46
    // hsv255 x 125-240 64-117
}
