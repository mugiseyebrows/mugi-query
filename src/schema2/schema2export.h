#ifndef SCHEMA2EXPORT_H
#define SCHEMA2EXPORT_H

#include <QString>
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

    Schema2Export(QGraphicsScene* scene, Schema2View* view, Schema2TablesModel* tables);

    void saveAs(bool clipboard, const QString &path, ExportFormat format, QWidget *widget);

protected:
    QGraphicsScene* mScene;
    Schema2View* mView;
    Schema2TablesModel* mTables;

};

#endif // SCHEMA2EXPORT_H
