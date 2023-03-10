#ifndef SCHEMA2RELATIONDIALOG_H
#define SCHEMA2RELATIONDIALOG_H

#include <QDialog>
class Schema2RelationModel;
class Schema2TableModel;

namespace Ui {
class Schema2RelationDialog;
}

class Schema2RelationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit Schema2RelationDialog(QWidget *parent = nullptr);
    ~Schema2RelationDialog();

    void init(Schema2TableModel* childTable, Schema2TableModel* parentTable,
                   Schema2RelationModel* model = 0);

    bool remove() const;

    QString childColumn() const;

    QString parentColumn() const;

    bool constrained() const;

    QString constraintName() const;
protected:
    Ui::Schema2RelationDialog *ui;
    Schema2RelationModel* mModel;
    bool mRemove;
private slots:
    void on_remove_clicked();
};

#endif // SCHEMA2RELATIONDIALOG_H
