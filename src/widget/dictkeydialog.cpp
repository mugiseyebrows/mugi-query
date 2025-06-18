#include "dictkeydialog.h"
#include "ui_dictkeydialog.h"

#include "checkablestringlistmodel.h"

DictKeyDialog::DictKeyDialog(QAbstractItemModel* model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DictKeyDialog)
{
    ui->setupUi(this);
    ui->view->setModel(model);
}

DictKeyDialog::~DictKeyDialog()
{
    delete ui;
}

QStringList DictKeyDialog::key() const
{
    CheckableStringListModel* model = qobject_cast<CheckableStringListModel*>(ui->view->model());
    return model->checked();
}
