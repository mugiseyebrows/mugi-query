#ifndef SCHEMA2DATA_H
#define SCHEMA2DATA_H

#include <QObject>

#include <QGraphicsScene>
#include <QGraphicsView>

class Schema2TableModel;
class Schema2TableView;
class Schema2Data;
class Schema2View;
class QGraphicsItem;
class Schema2RelationModel;
class Schema2RelationItem;
class Schema2TableItem;

#include <QHash>

class Schema2Data : public QObject
{
    Q_OBJECT
public:

    static Schema2Data* instance(const QString& connectionName, QObject *parent = nullptr);

    static QHash<QString, Schema2Data*> mData;

    void pull();

    void push();

    void save();

    void load();

    bool hasPendingChanges() const;

    Schema2View* view();

protected:
    Schema2Data(const QString& connectionName, QObject *parent = nullptr);

    QString mConnectionName;

    QGraphicsScene* mScene;

    Schema2View* mView;

    QHash<QString, Schema2TableItem*> mTableItems;

    QHash<QString, Schema2TableModel*> mTables;

    QHash<QString, Schema2TableView*> mViews;

    QHash<QString, QPointF> mTablePos;

    QHash<QStringList, Schema2RelationModel*> mRelationModels;

    QHash<QStringList, Schema2RelationItem*> mRelationItems;

    void pullTables();
    void pullRelations();
    void pullRelationsMysql();

signals:

};

#endif // SCHEMA2DATA_H
