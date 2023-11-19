#include "schema2view.h"
#include "ui_schema2view.h"

#include "schema2data.h"
#include "schema2tableitem.h"
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QInputDialog>
#include "schema2tablesmodel.h"
#include "automation.h"
#include "schema2toolbar.h"
#include "schema2zoomtoolbar.h"
#include "choicedialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include "exportdialog.h"

Schema2View::Schema2View(QWidget *parent) :
    mData(0),
    QWidget(parent),

    ui(new Ui::Schema2View)
{
    ui->setupUi(this);


    connect(ui->toolbar, &Schema2Toolbar::clearTableStack, [=](){
        mTableStack.clear();
    });

    connect(ui->zoom, &Schema2ZoomToolbar::zoom, [=](double scale){
       ui->view->setTransform(QTransform().scale(scale, scale));
    });

    connect(ui->actions, &Schema2ActionToolbar::action, [=](int action){

        switch(action) {
        case Schema2ActionToolbar::Create: onCreate(); break;
        case Schema2ActionToolbar::Arrange: onArrange(); break;
        case Schema2ActionToolbar::Script: onScript(); break;
        case Schema2ActionToolbar::Save: onSave(); break;
        }

    });

    connect(ui->sync, SIGNAL(pull()),this, SLOT(onPull()));
    connect(ui->sync, SIGNAL(push()),this, SLOT(onPush()));
}


void Schema2View::onPull()
{
    mData->pull();
}

void Schema2View::onPush()
{
    mData->push(this);
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

#include "schema2/schema2toolbar.h"

void Schema2View::onTableClicked(QString tableName)
{

    //qDebug() << "onTableClicked" << tableName << mMode;

    Schema2Toolbar::Mode mode = ui->toolbar->mode();

    switch (mode) {
    case Schema2Toolbar::ModeNone: break;
    case Schema2Toolbar::ModeMove: break;
    case Schema2Toolbar::ModeSelect: mData->selectOrDeselect(tableName); break;
    case Schema2Toolbar::ModeRelate:
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
    case Schema2Toolbar::ModeUnrelate:
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
    case Schema2Toolbar::ModeDrop:
        mData->dropTableDialog(tableName, this);
        break;
    case Schema2Toolbar::ModeAlter: mData->showAlterView(tableName); break;
    case Schema2Toolbar::ModeInsert: mData->showInsertView(tableName); break;
    }
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

QGraphicsScene *Schema2View::scene()
{
    return mData->scene();
}

Schema2TablesModel *Schema2View::tables() const
{
    return mData->tables();
}

void Schema2View::onCreate()
{
    QString tableName = QInputDialog::getText(this, "Table Name", "Name");
    if (tableName.isEmpty()) {
        return;
    }
    createTable(tableName);
}



void Schema2View::onArrange()
{
    ChoiceDialog dialog(this);
    dialog.init({"All", "Selected"});
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    bool all = dialog.checkedIndex() == 0;
    mData->arrange(all);
}


void Schema2View::on_filterLine_textChanged(const QString &text)
{
    mData->selectProxyModel()->setFilterRegularExpression(QRegularExpression(text, QRegularExpression::CaseInsensitiveOption));
}

void Schema2View::onScript()
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

void Schema2View::onSave()
{

    ExportDialog dialog(this);

    Automation::instance()->beforeDialog(&dialog);

    dialog.setName(mData->connectionName());
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    bool clipboard = dialog.clipboard();
    QString path = dialog.path();

    QRectF rect;
    if (dialog.cropViewport()) {
        auto* view = ui->view;
        rect = view->mapToScene(view->viewport()->geometry()).boundingRect();
    } else {
        rect = sceneRect();
    }
    if (!clipboard && QFile::exists(path)) {
        int res = QMessageBox::question(this, "Overwrite", "File exists, overwrite?", QMessageBox::No, QMessageBox::Yes);
        if (res != QMessageBox::Yes) {
            return;
        }
    }
    mData->saveAs(clipboard, path, rect, dialog.itemsSelected(), dialog.format(), this);
    Automation::instance()->afterDialog(&dialog);
}


