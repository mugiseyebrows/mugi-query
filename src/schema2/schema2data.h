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
class Schema2RelationItem2;

#include "schema2join.h"
#include "hash.h"
#include "schema2status.h"

class Schema2Data : public QObject
{
    Q_OBJECT
public:
    static QHash<int, QString> mOdbcTypes;

    static Schema2Data* instance(const QString& connectionName, QObject *parent = nullptr);

    static QHash<QString, Schema2Data*> mData;

    void pull();

    void push(QWidget* widget);

    void save();

    void load();

    bool hasPendingChanges() const;

    Schema2View* view();

    QGraphicsScene* scene();

    void showRelationDialog(const QString &childTable, const QString &parentTable, QWidget* widget);

    void showAlterView(const QString& tableName);

    void showInsertView(const QString &tableName);

    void loadPos();

    void savePos();

    void arrange();

    QList<Schema2Join> findJoin(const QStringList& join);

    QSortFilterProxyModel* selectProxyModel();

    void selectOrDeselect(const QString& table);

    void showRelationsListDialog(QWidget *widget);

    void createTable(const QString& name);

    Schema2TableItem* tableItem(const QString& name) const {
        return mTableItems.get(name);
    }

    QStringList dataTypes() const;
protected:
    Schema2Data(const QString& connectionName, QObject *parent = nullptr);

    QString mConnectionName;

    QGraphicsScene* mScene;

    ClickListener* mClickLister;

    Schema2View* mView;

    StringHash<Schema2TableItem*> mTableItems;

    StringHash<Schema2TableModel*> mTableModels;

    //QHash<QString, Schema2TableView*> mViews;

    StringHash<Schema2AlterView*> mAlterViews;

    StringHash<QPointF> mTablePos;

    //StringListHash<Schema2RelationModel*> mRelationModels;

    QList<Schema2RelationItem2*> mRelationItems;

    QList<Schema2TableItem*> mSetPosQueue;

    QList<Schema2RelationModel*> mRemoveRelationsQueue;

    CheckableStringListModel* mSelectModel;

    QSortFilterProxyModel* mSelectProxyModel;


    void pullTables();
    void pullIndexes();
    void pullRelations();

    void pullTablesOdbc();
    void pullTablesMysql();
    void pullTablesOther();

    void pullIndexesOdbc();
    void pullIndexesMysql();

    void pullRelationsMysql();
    void pullRelationsOdbc();

    //void unoverlapTables();
    void setTableItemsPos();


    void relationPulled(const QString &constraintName, const QString &childTable, const QStringList &childColumns, const QString &parentTable, const QStringList &parentColumns);

    void tablePulled(const QString &tableName, Status status);
signals:
    void tableClicked(QString);
protected slots:
    void onSelectModelChanged(QModelIndex, QModelIndex);
};

#endif // SCHEMA2DATA_H