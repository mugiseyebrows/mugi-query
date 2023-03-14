#ifndef SCHEMA2RELATIONDIALOG2_H
#define SCHEMA2RELATIONDIALOG2_H

#include <QDialog>
class Schema2TableModel;
class Schema2Relation;

namespace Ui {
class Schema2RelationDialog2;
}

class Schema2RelationDialog2 : public QDialog
{
    Q_OBJECT

public:
    explicit Schema2RelationDialog2(QWidget *parent = nullptr);
    ~Schema2RelationDialog2();

    void init(Schema2TableModel* childTable,
              Schema2TableModel* parentTable,
              Schema2Relation* relation);

private:
    Ui::Schema2RelationDialog2 *ui;
    Schema2TableModel* mChildTable;
    Schema2TableModel* mParentTable;
    Schema2Relation* mRelation;
};

#endif // SCHEMA2RELATIONDIALOG2_H
