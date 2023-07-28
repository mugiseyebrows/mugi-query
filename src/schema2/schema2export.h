#ifndef SCHEMA2EXPORT_H
#define SCHEMA2EXPORT_H

#include <QString>
#include <QRectF>
class QWidget;
class QGraphicsScene;
class Schema2View;
class Schema2TablesModel;

class Schema2Export
{
public:
    enum ExportFormat {
        PngFormat,
        SvgFormat,
        DotFormat,
        DbioFormat,
    };

    Schema2Export(Schema2View* view);

    void saveAs(bool clipboard, const QString &path, const QRectF& crop, bool onlySelected,
                ExportFormat format, QWidget *widget);

protected:
    Schema2View* mView;
};

#endif // SCHEMA2EXPORT_H
