#ifndef SCHEMA2ALTERVIEW_H
#define SCHEMA2ALTERVIEW_H

#include <QWidget>

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

    void init(Schema2TableModel *model);
protected:
    Schema2TableModel* mModel;

    void initColumns(Schema2TableModel *model);
    void initRelations(Schema2TableModel *model);
    void initIndexes(Schema2TableModel *model);
private slots:
    void on_createRelation_clicked();

    void on_createIndex_clicked();

private:
    Ui::Schema2AlterView *ui;
};

#endif // SCHEMA2ALTERVIEW_H
