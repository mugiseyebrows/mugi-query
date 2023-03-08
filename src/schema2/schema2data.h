#ifndef SCHEMA2DATA_H
#define SCHEMA2DATA_H

#include <QObject>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QModelIndex>

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
class CheckableStringListModel;
class QSortFilterProxyModel;

#include <QHash>
#include "schema2join.h"

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

    QList<Schema2Join> findJoin(const QStringList& join);

    QSortFilterProxyModel* selectProxyModel() {
        return mSelectProxyModel;
    }

    void selectOrDeselect(const QString& table);


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

    CheckableStringListModel* mSelectModel;

    QSortFilterProxyModel* mSelectProxyModel;

    void pullTables();
    void pullRelations();
    void pullRelationsMysql();

    //void unoverlapTables();
    void setTableItemsPos();
    void pullRelationsOdbc();
signals:
    void tableClicked(QString);
protected slots:
    void onSelectModelChanged(QModelIndex, QModelIndex);
};

#endif // SCHEMA2DATA_H
