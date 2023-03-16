#ifndef SCHEMA2ALTERVIEW_H
#define SCHEMA2ALTERVIEW_H

#include <QWidget>
#include "hash.h"

namespace Ui {
class Schema2AlterView;
}

class Schema2TableModel;
class Schema2RelationModel;

class Schema2AlterView : public QWidget
{
    Q_OBJECT

public:
    explicit Schema2AlterView(QWidget *parent = nullptr);
    ~Schema2AlterView();

    //void setModel(Schema2TableModel* model);

    void init(const StringHash<Schema2TableModel *> &tableModels, Schema2TableModel *model, const QStringList &types);
protected:
    Schema2TableModel* mModel;
    StringHash<Schema2TableModel *> mTableModels;
    QStringList mTypes;

    void initColumns();
    void initRelations();
    void initIndexes();
private slots:
    void on_createRelation_clicked();

    void on_createIndex_clicked();
signals:
    void createRelation(QString childTable, QStringList childColumns, QString parentTable);
    void editRelation();

private:
    Ui::Schema2AlterView *ui;
};

#endif // SCHEMA2ALTERVIEW_H
