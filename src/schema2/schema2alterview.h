#ifndef SCHEMA2ALTERVIEW_H
#define SCHEMA2ALTERVIEW_H

#include <QWidget>
#include "hash.h"

namespace Ui {
class Schema2AlterView;
}

class Schema2TableModel;
class Schema2RelationModel;
class Schema2Data;

class Schema2AlterView : public QWidget
{
    Q_OBJECT

public:
    explicit Schema2AlterView(QWidget *parent = nullptr);
    ~Schema2AlterView();

    //void setModel(Schema2TableModel* model);

    void init(Schema2Data* data,
              const StringHash<Schema2TableModel *> &tableModels,
              Schema2TableModel *model,
              const QStringList &types);
protected:
    Schema2Data* mData;

    Schema2TableModel* mModel;
    StringHash<Schema2TableModel *> mTableModels;
    QStringList mTypes;

    void initColumns();
    void initRelations();
    void initIndexes();
    QStringList selectedFields() const;
    void createIndex(bool primary, bool unique);
private slots:
    void on_createRelation_clicked();

    void on_createIndex_clicked();
    void on_createPrimaryKey_clicked();

    void on_createUniqueIndex_clicked();

signals:
    void createRelation(QString childTable, QStringList childColumns, QString parentTable);
    void editRelation();

private:
    Ui::Schema2AlterView *ui;
};

#endif // SCHEMA2ALTERVIEW_H
