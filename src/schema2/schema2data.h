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
class ClickListener;
class Schema2AlterView;

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

    QGraphicsScene* scene();

    void showRelationDialog(const QString &childTable, const QString &parentTable, QWidget* parent);

    void showAlterView(const QString& tableName);

    void showInsertView(const QString &tableName);

    void loadPos();

    void savePos();

    void arrange();

protected:
    Schema2Data(const QString& connectionName, QObject *parent = nullptr);

    QString mConnectionName;

    QGraphicsScene* mScene;

    ClickListener* mClickLister;

    Schema2View* mView;

    QHash<QString, Schema2TableItem*> mTableItems;

    QHash<QString, Schema2TableModel*> mTableModels;

    //QHash<QString, Schema2TableView*> mViews;

    QHash<QString, Schema2AlterView*> mAlterViews;

    QHash<QString, QPointF> mTablePos;

    QHash<QStringList, Schema2RelationModel*> mRelationModels;

    QHash<QStringList, Schema2RelationItem*> mRelationItems;

    QList<Schema2TableItem*> mSetPosQueue;

    QList<Schema2RelationModel*> mRemoveRelationsQueue;

    void pullTables();
    void pullRelations();
    void pullRelationsMysql();

    //void unoverlapTables();
    void setTableItemsPos();
signals:
    void tableClicked(QString);
};

#endif // SCHEMA2DATA_H
