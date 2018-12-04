#include "savedatadialog.h"
#include "ui_savedatadialog.h"

#include "checkablestringlistmodel.h"

SaveDataDialog::SaveDataDialog(const QStringList &columns, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveDataDialog)
{
    ui->setupUi(this);

    CheckableStringListModel* m1 = new CheckableStringListModel(columns,this);
    CheckableStringListModel* m2 = new CheckableStringListModel(columns,this);

    m1->setAllChecked();
    ui->data->setModel(m1);
    ui->keys->setModel(m2);

}

SaveDataDialog::~SaveDataDialog()
{
    delete ui;
}

void SaveDataDialog::on_allData_clicked()
{

}

void SaveDataDialog::on_noneData_clicked()
{

}

void SaveDataDialog::on_allKeys_clicked()
{

}

void SaveDataDialog::on_noneKeys_clicked()
{

}

