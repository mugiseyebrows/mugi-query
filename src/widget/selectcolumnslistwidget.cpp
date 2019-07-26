#include "selectcolumnslistwidget.h"
#include "ui_selectcolumnslistwidget.h"

#include "model/dataimportcolumnmodel.h"

SelectColumnsListWidget::SelectColumnsListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectColumnsListWidget)
{
    ui->setupUi(this);
}

SelectColumnsListWidget::~SelectColumnsListWidget()
{
    delete ui;
}

void SelectColumnsListWidget::setFields(const QStringList &fields, const QList<QVariant::Type>& types)
{
    DataImportColumnModel* model = new DataImportColumnModel(fields, types, this);
    ui->columns->setModel(model);
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this,SIGNAL(dataChanged(QModelIndex,QModelIndex)));
}

void SelectColumnsListWidget::setLabelText(const QString &text)
{
    ui->label->setText(text);
    ui->label->setVisible(!text.isEmpty());
}


DataImportColumnModel *SelectColumnsListWidget::model() const
{
    return qobject_cast<DataImportColumnModel*>(ui->columns->model());
}

bool SelectColumnsListWidget::hasAnyChecked() const {
    return model() && model()->hasAnyChecked();
}

void SelectColumnsListWidget::setAllChecked() {
    if (!model()) {
        return;
    }
    model()->setAllChecked();
}

void SelectColumnsListWidget::setAllUnchecked() {
    if (!model()) {
        return;
    }
    model()->setAllUnchecked();
}

void SelectColumnsListWidget::on_all_clicked()
{
    setAllChecked();
}

void SelectColumnsListWidget::on_none_clicked()
{
    setAllUnchecked();
}
