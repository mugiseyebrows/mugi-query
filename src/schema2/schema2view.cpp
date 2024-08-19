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
#include "schema2/schema2toolbar.h"
#include "choicedialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include "exportdialog.h"
#include "schema2tablemodel.h"
#include "schema2relationsmodel.h"
#include "style.h"

Schema2View::Schema2View(QWidget *parent) :
    mData(0),
    QWidget(parent),

    ui(new Ui::Schema2View)
{
    ui->setupUi(this);


    connect(ui->toolbar, &Schema2Toolbar::modeChanged, [=](int mode){
        bool visible = mode == Schema2Toolbar::ModeRelate || mode == Schema2Toolbar::ModeUnrelate;
        ui->relate->setVisible(visible);
        ui->relate->clear();
        setTablesMovable(mode == Schema2Toolbar::ModeMove);
    });

    connect(ui->relate, &RelateToolWidget::selected, [=](QString parentTable, QString childTable){
        Schema2Toolbar::Mode mode = ui->toolbar->mode();
        if (mode == Schema2Toolbar::ModeRelate) {
            mData->createRelationDialog(childTable, parentTable, this);
            ui->relate->clear();
        } else if (mode == Schema2Toolbar::ModeUnrelate) {
            mData->dropRelationDialog(childTable, parentTable, this);
            ui->relate->clear();
        }
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

    ui->relate->hide();

    QAction* checkRelated = new QAction("Check related", this);
    ui->filterView->addAction(checkRelated);
    //ui->filterView->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(checkRelated, &QAction::triggered, [=](){
        auto checked = mData->tables()->checked(true);
        QStringList related;
        QList<Schema2TableModel *> tables = mData->tables()->tables();
        for(auto* table: tables) {
            Schema2RelationsModel* relationsModel = table->relations();
            for(int row=0;row<relationsModel->rowCount();row++) {
                Schema2Relation* relation = relationsModel->at(row);
                auto parent = relation->parentTable();
                auto child = relation->childTable();
                if (checked.contains(parent)) {
                    related.append(child);
                }
                if (checked.contains(child)) {
                    related.append(parent);
                }
            }
        }
        mData->tables()->setChecked(related, true);
    });
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

    connect(mData,SIGNAL(tableClicked(QString, QPointF)),this,SLOT(onTableClicked(QString, QPointF)));

    ui->view->setScene(mData->scene());

    ui->filterView->setModel(mData->selectProxyModel());

    connect(ui->filterView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(onFiterViewCurrentChanged(QModelIndex,QModelIndex)));
}

void Schema2View::setTablesMovable(bool on) {
    QList<QGraphicsItem *> items = scene()->items();
    for(auto* item: items) {
        Schema2TableItem* table = qgraphicsitem_cast<Schema2TableItem*>(item);
        if (table) {
            //qDebug() << table;
            table->setFlag(QGraphicsItem::ItemIsMovable, on);
        }
    }
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



QList<Schema2TableItem*> Schema2View::tablesAt(const QPointF& pos) const {
    QList<QGraphicsItem*> items = scene()->items(pos);
    QList<Schema2TableItem*> res;
    for(auto* item: items) {
        Schema2TableItem* table = qgraphicsitem_cast<Schema2TableItem*>(item);
        if (table) {
            res.append(table);
        }
    }
    return res;
}

void Schema2View::onTableClicked(QString tableName_, QPointF scenePos)
{

    //qDebug() << "onTableClicked" << tableName << mMode;


    auto tables = tablesAt(scenePos);
    /*for(auto* table: tables) {
        qDebug() << "table at pos" << scenePos << table->tableName() << table;
    }
    qDebug() << "--";*/
    // todo figure out, file bug report
    QString tableName;
    if (tables.size() == 1) {
        tableName = tables[0]->tableName();
    } else {
        tableName = tableName_;
    }

    Schema2Toolbar::Mode mode = ui->toolbar->mode();

    switch (mode) {
    case Schema2Toolbar::ModeNone: break;
    case Schema2Toolbar::ModeMove: break;
    case Schema2Toolbar::ModeSelect: mData->selectOrDeselect(tableName); break;
    case Schema2Toolbar::ModeRelate:
        /*mTableStack.append(tableName);
        if (mTableStack.size() == 2) {
            QString childTable = mTableStack[0];
            QString parentTable = mTableStack[1];
            //if (childTable != parentTable) {
                mData->createRelationDialog(childTable, parentTable, this);
            //}
            mTableStack.clear();
        }*/
        ui->relate->push(tableName);
        break;
    case Schema2Toolbar::ModeUnrelate:
        /*mTableStack.append(tableName);
        if (mTableStack.size() == 2) {
            QString childTable = mTableStack[0];
            QString parentTable = mTableStack[1];
            //if (childTable != parentTable) {
                mData->dropRelationDialog(childTable, parentTable, this);
            //}
            mTableStack.clear();
        }*/
        ui->relate->push(tableName);
        break;
    case Schema2Toolbar::ModeDrop:
        mData->dropTableDialog(tableName, this);
        break;
    case Schema2Toolbar::ModeAlter: mData->showAlterView(tableName); break;
    case Schema2Toolbar::ModeInsert: mData->showDataImportWidget(tableName); break;
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

QGraphicsScene *Schema2View::scene() const
{
    return mData->scene();
}

Schema2TablesModel *Schema2View::tables() const
{
    return mData->tables();
}

void Schema2View::updateView()
{
    ui->view->viewport()->update();
    ui->view->setBackgroundBrush(Style::current.BackgroundColor);
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
    mData->tables()->setChecked(true);
}

void Schema2View::on_uncheckAll_clicked()
{
    mData->tables()->setChecked(false);
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

#include "stylewidget.h"

void Schema2View::on_style_clicked()
{
    StyleWidget* widget = new StyleWidget();
    widget->show();
}

