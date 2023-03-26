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

    QString relationName() const;

    QStringList childColumns() const;

    QStringList parentColumns() const;

    bool constrained() const;

    void init(Schema2TableModel *childTable, Schema2TableModel *parentTable, const QString &relationName, const QStringList &childColumns, const QStringList &parentColumns);

private:
    Ui::Schema2RelationDialog2 *ui;
    Schema2TableModel* mChildTable;
    Schema2TableModel* mParentTable;
};

#endif // SCHEMA2RELATIONDIALOG2_H
