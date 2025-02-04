#ifndef SCHEMACOLOR_H
#define SCHEMACOLOR_H

#include <QColor>
#include <QHash>

class SchemaColor {
public:

    QColor color(const QString& schema);

    static SchemaColor* instance() {
        if (!mInstance) {
            mInstance = new SchemaColor();
        }
        return mInstance;
    }
protected:
    SchemaColor();
    static SchemaColor* mInstance;
    QList<QColor> mPalette;
    QHash<QString, int> mColors;
    int mIndex = 0;
};


#endif // SCHEMACOLOR_H
