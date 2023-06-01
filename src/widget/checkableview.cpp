#include "checkableview.h"
#include "ui_checkableview.h"

#include "model/checkablemodel.h"
#include <QSortFilterProxyModel>

CheckableView::CheckableView(QWidget *parent, const QStringList &items) :
    QWidget(parent),
    ui(new Ui::CheckableView),
    mProxyModel(new QSortFilterProxyModel(this)),
    mModel(new CheckableModel(this))
{
    ui->setupUi(this);
    mProxyModel->setSourceModel(mModel);
    ui->tables->setModel(mProxyModel);
    append(items);
}

CheckableView::~CheckableView()
{
    delete ui;
}

void CheckableView::append(const QStringList &items)
{
    mModel->append(items);
}

QStringList CheckableView::checked() const
{
    return mModel->checked();
}

static void setChecked(QAbstractItemModel* model, Qt::CheckState state) {
    for(int row=0;row<model->rowCount();row++) {
        auto index = model->index(row, 0);
        model->setData(index, state, Qt::CheckStateRole);
    }
}

void CheckableView::on_checkAll_clicked()
{
    setChecked(mProxyModel, Qt::Checked);
}

void CheckableView::on_uncheckAll_clicked()
{
    setChecked(mProxyModel, Qt::Unchecked);
}

void CheckableView::on_filter_textChanged(const QString &text)
{
    mProxyModel->setFilterFixedString(text);
}
