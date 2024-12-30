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
class Schema2TableItem;
class ClickListener;
class Schema2AlterView;
class CheckableStringListModel;
class QSortFilterProxyModel;
class Schema2RelationItem2;
class Schema2Relation;
class Schema2TablesModel;
class QSqlDriver;
class DataImportWidget2;
class OdbcUri;
class QSqlDatabase;

#include "schema2join.h"
#include "hash.h"
#include "schema2status.h"
#include "schema2export.h"
#include "sdata.h"

class Schema2Data : public QObject
{
    Q_OBJECT
public:


    static QHash<int, QString> mOdbcTypes;

    static bool mDontAskOnDropTable;

    static bool mDontAskOnDropRelation;

    static Schema2Data* instance(const QString& connectionName, QObject *parent = nullptr);

    static QHash<QString, Schema2Data*> mData;

    void pull();

    void push(QWidget* widget);

    void save();

    void load();

    bool hasPendingChanges() const;

    Schema2View* view();

    QGraphicsScene* scene() const;

    void showAlterView(const SName &tableName);

    void showDataImportWidget(const SName &tableName);

    void arrange(bool all);

    QList<Schema2Join> findJoin(const QStringList& join);

    QSortFilterProxyModel* selectProxyModel();

    void selectOrDeselect(const SName &table);

    //void showRelationsListDialog(QWidget *widget);

    Schema2TableModel* createTable(const SName &name);

    Schema2TableItem* tableItem(const QString& name) const;

    QStringList dataTypes() const;

    //void indexPulled(const QString indexName, const QString &tableName, const QStringList &columns, bool primary, bool unique, Status status);

    void createRelationDialog(Schema2TableModel* childTable, QStringList childColumns, QString parentTable);
    void createRelationDialog(const SName &childTable, const SName &parentTable, QWidget* widget);
    void editRelationDialog(Schema2Relation *relation, QWidget *widget);
    void dropRelationDialog(Schema2Relation *relation, QWidget *widget);
    void dropRelationDialog(const SName &childTable, const SName &parentTable, QWidget *widget);
    void dropTableDialog(const SName &table, QWidget *widget);
    void dropIndexDialog(QWidget *widget);

    QString connectionName() const {
        return mConnectionName;
    }

    QSqlDatabase database() const;

    QString driverName() const;

    Schema2AlterView* alterView(const QString& table);

    Schema2TablesModel* tables() const;

    void scriptDialog(QWidget* widget);

    QSqlDriver *driver() const;

    void showDataStatistics(QWidget* widget);

    void copyRelationsToClipboard(QWidget *widget);
    void copyPrimaryKeysToClipboard(QWidget *widget);

    void saveAs(bool clipboard, const QString& path, const QRectF &rect, bool onlySelected, Schema2Export::ExportFormat format, QWidget *widget);

    void tableRenamed(const SName &tableName, const SName &tableNamePrev);

protected:
    Schema2Data(const QString& connectionName, QObject *parent = nullptr);

    QString mConnectionName;

    QGraphicsScene* mScene;

    ClickListener* mClickLister;

    Schema2View* mView;

    Schema2TablesModel* mTables;

    //QHash<QString, Schema2TableView*> mViews;

    QHash<SName, Schema2AlterView*> mAlterViews;

    QHash<SName, DataImportWidget2*> mDataImportWidgets;

    //QList<QPair<QString, Schema2Relation*>> mDropRelationsQueue;

    QList<Schema2TableModel*> mDropTableQueue;

    //CheckableStringListModel* mSelectModel;

    QSortFilterProxyModel* mSelectProxyModel;


    void pullTables();
    void pullIndexes();
    void pullRelations();

    void pullTablesOdbcAccess(const OdbcUri &uri);
    void pullTablesMysql();
    void pullTablesOther();

    void pullIndexesOdbcAccess(const OdbcUri &uri);
    void pullIndexesMysql();

    void pullRelationsMysql();
    void pullRelationsOdbcAccess(const OdbcUri &uri);

    //void unoverlapTables();

    //void relationPulled(const QString &constraintName, const QString &childTable, const QStringList &childColumns, const QString &parentTable, const QStringList &parentColumns, bool constrained, Status status);

    void tablePulled(const QString &tableName, Status status);
    //QStringList guessParentColumns(QString childTable, QStringList childColumns, QString parentTable);
signals:
    void tableClicked(SName, QPointF);
protected slots:
    void onSelectModelChanged(QModelIndex, QModelIndex);

};

#endif // SCHEMA2DATA_H
