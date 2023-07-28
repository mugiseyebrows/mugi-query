#include "schema2view.h"
#include "ui_schema2view.h"

#include "schema2data.h"
#include "schema2tableitem.h"
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QInputDialog>
#include "schema2tablesmodel.h"

QList<double> Schema2View::mScales = {0.1, 0.25, 0.5, 1.0};
QStringList Schema2View::mScalesText = {"10%", "25%", "50%", "100%"};

Schema2View::Schema2View(QWidget *parent) :
    mMode(ModeNone),
    mData(0),
    QWidget(parent),
    mScaleIndex(mScales.size() - 1),
    ui(new Ui::Schema2View)
{
    ui->setupUi(this);
    ui->scale->addItems(mScalesText);
    ui->scale->setCurrentIndex(mScaleIndex);
}

Schema2View::~Schema2View()
{
    delete ui;
}

void Schema2View::setData(Schema2Data *data)
{
    mData = data;

    connect(mData,SIGNAL(tableClicked(QString)),this,SLOT(onTableClicked(QString)));

    ui->view->setScene(mData->scene());

    ui->filterView->setModel(mData->selectProxyModel());

    connect(ui->filterView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(onFiterViewCurrentChanged(QModelIndex,QModelIndex)));

}

Schema2Data *Schema2View::data() const {
    return mData;
}

void Schema2View::onFiterViewCurrentChanged(QModelIndex index,QModelIndex) {

    if (!index.isValid()) {
        return;
    }

    QString name = index.data().toString();
    //qDebug() << "onFiterViewCurrentChanged" << name;
    Schema2TableItem* item = mData->tableItem(name);
    if (!item) {
        qDebug() << "!item" << name << __FILE__ << __LINE__;
        return;
    }
    ui->view->centerOn(item->sceneBoundingRect().center());
}

void Schema2View::onTableClicked(QString tableName)
{

    //qDebug() << "onTableClicked" << tableName << mMode;

    switch (mMode) {
    case ModeNone: break;
    case ModeMove: break;
    case ModeSelect: mData->selectOrDeselect(tableName); break;
    case ModeRelate:
        mTableStack.append(tableName);
        if (mTableStack.size() == 2) {
            QString childTable = mTableStack[0];
            QString parentTable = mTableStack[1];
            //if (childTable != parentTable) {
                mData->createRelationDialog(childTable, parentTable, this);
            //}
            mTableStack.clear();
        }
        break;
    case ModeUnrelate:
        mTableStack.append(tableName);
        if (mTableStack.size() == 2) {
            QString childTable = mTableStack[0];
            QString parentTable = mTableStack[1];
            //if (childTable != parentTable) {
                mData->dropRelationDialog(childTable, parentTable, this);
            //}
            mTableStack.clear();
        }
        break;
    case ModeDrop:
        mData->dropTableDialog(tableName, this);
        break;
    case ModeAlter: mData->showAlterView(tableName); break;
    case ModeInsert: mData->showInsertView(tableName); break;
    }
}

void Schema2View::uncheckAllExcept(QPushButton* checked) {
    QList<QPushButton*> buttons = {ui->move, ui->select, ui->relate, ui->alter, ui->insert, ui->unrelate, ui->drop};
    for(QPushButton* button: buttons) {
        if (button == checked) {
            continue;
        }
        button->setChecked(false);
    }
}

void Schema2View::on_select_clicked(bool checked)
{
    uncheckAllExcept(ui->select);
    mMode = checked ? ModeSelect : ModeNone;
}

void Schema2View::on_move_clicked(bool checked)
{
    uncheckAllExcept(ui->move);
    mMode = checked ? ModeMove : ModeNone;
}

void Schema2View::on_relate_clicked(bool checked)
{
    uncheckAllExcept(ui->relate);
    mMode = checked ? ModeRelate : ModeNone;
    mTableStack.clear();
}

void Schema2View::on_unrelate_clicked(bool checked)
{
    uncheckAllExcept(ui->unrelate);
    mMode = checked ? ModeUnrelate : ModeNone;
    mTableStack.clear();
}

void Schema2View::on_drop_clicked(bool checked)
{
    uncheckAllExcept(ui->drop);
    mMode = checked ? ModeDrop : ModeNone;
}

void Schema2View::on_alter_clicked(bool checked)
{
    uncheckAllExcept(ui->alter);
    mMode = checked ? ModeAlter : ModeNone;
}

void Schema2View::on_insert_clicked(bool checked)
{
    uncheckAllExcept(ui->insert);
    mMode = checked ? ModeInsert : ModeNone;
}

Schema2TableModel* Schema2View::createTable(const QString& tableName) {
    Schema2TableModel* table = mData->createTable(tableName);
    mData->showAlterView(tableName);
    return table;
}

QRectF Schema2View::sceneRect() const
{
    return ui->view->sceneRect();
}

void Schema2View::on_create_clicked()
{
    QString tableName = QInputDialog::getText(this, "Table Name", "Name");
    if (tableName.isEmpty()) {
        return;
    }
    createTable(tableName);
}

#include "choicedialog.h"

void Schema2View::on_arrange_clicked()
{
    ChoiceDialog dialog(this);
    dialog.init({"All", "Selected"});
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    bool all = dialog.checkedIndex() == 0;

    mData->arrange(all);


}

void Schema2View::on_pull_clicked()
{
    mData->pull();
}

void Schema2View::on_push_clicked()
{
    mData->push(this);
}

void Schema2View::on_zoomOut_clicked()
{
    int currentIndex = ui->scale->currentIndex();
    if (currentIndex - 1 > -1) {
        ui->scale->setCurrentIndex(currentIndex - 1);
    }
}

void Schema2View::on_zoomIn_clicked()
{
    int currentIndex = ui->scale->currentIndex();
    if (currentIndex + 1 < mScales.size()) {
        ui->scale->setCurrentIndex(currentIndex + 1);
    }
}

void Schema2View::on_scale_currentIndexChanged(int index)
{
    double scale = mScales[index];
    ui->view->setTransform(QTransform().scale(scale, scale));
}

void Schema2View::on_filterLine_textChanged(const QString &text)
{
    mData->selectProxyModel()->setFilterRegularExpression(QRegularExpression(text, QRegularExpression::CaseInsensitiveOption));
}

void Schema2View::on_script_clicked()
{
    mData->scriptDialog(this);
}

void Schema2View::on_checkAll_clicked()
{
    mData->tables()->setAllGrayed(false);
}

void Schema2View::on_uncheckAll_clicked()
{
    mData->tables()->setAllGrayed(true);
}

void Schema2View::on_grayed_clicked()
{
    mData->tables()->setUncheckedMode(UncheckedGrayed);
}

void Schema2View::on_invisible_clicked()
{
    mData->tables()->setUncheckedMode(UncheckedInvisible);
}

#include <QFileDialog>

void Schema2View::on_saveAs_clicked()
{
    ChoiceDialog dialog;
    dialog.init({"Svg", "Png", "Dot", "dbdiagram.io"}, 0);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QStringList filters = {
        "Svg files (*.svg)",
        "Png files (*.png)",
        "Dot files (*.dot)",
        "Txt files (*.txt)",
    };

    QList<Schema2Data::OutputFormat> formats = {
        Schema2Data::SvgFormat,
        Schema2Data::PngFormat,
        Schema2Data::DotFormat,
        Schema2Data::DbioFormat,
    };

    auto filter = filters[dialog.checkedIndex()];
    QString path = QFileDialog::getSaveFileName(this, QString(), QString(), filter);
    if (path.isEmpty()) {
        return;
    }
    Schema2Data::OutputFormat format = formats[dialog.checkedIndex()];
    mData->saveAs(path, format, this);
}

