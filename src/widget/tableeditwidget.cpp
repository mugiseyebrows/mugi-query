#include "tableeditwidget.h"
#include "ui_tableeditwidget.h"

#include <QSqlTableModel>
#include <richheaderview/richheaderdata.h>
#include <richheaderview/richheaderview.h>
#include <QPushButton>
#include <QLineEdit>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QSqlField>

TableEditWidget::TableEditWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TableEditWidget)
{
    ui->setupUi(this);
}

TableEditWidget::~TableEditWidget()
{
    delete ui;
}

void TableEditWidget::init(QSqlDatabase db, const SName &table)
{
    QSqlTableModel* model = new QSqlTableModel(this, db);
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->setTable(table.fullname());
    model->select();

    mModel = model;

    ui->view->setModel(model);

    RichHeaderView* view = new RichHeaderView(Qt::Horizontal, ui->view);
    RichHeaderData data = view->data();
    //data.stretchFixed(false);
    data.subsectionSizes({30, 30});

    QWidget* viewport = ui->view->viewport();

    for(int column = 0;column<model->columnCount(); column+=1) {

        QString text = model->headerData(column, Qt::Horizontal).toString();

        QLineEdit* edit = new QLineEdit(viewport);
        mEdits.append(edit);

        data.cell(0, column).widget(new QPushButton(text, viewport)).paddingLeft(5);
        data.cell(1, column).widget(edit);

        connect(edit, &QLineEdit::textChanged, this, &TableEditWidget::onFilterChanged);
    }

    ui->view->setHorizontalHeader(view);

    // show widgets
    view->update();
}

void TableEditWidget::onFilterChanged(const QString&) {

    QStringList conditions;

    QSqlRecord rec = mModel->record();

    QSqlDatabase db = mModel->database();
    QSqlDriver* driver = db.driver();

    for(int i=0;i<mEdits.size();i++) {
        QString text = mEdits[i]->text();
        if (!text.isEmpty()) {
            QString colName = rec.fieldName(i);
            QSqlField field = rec.field(i);
            field.setValue("%" + text + "%");
            QString condition = QString("%1 like %2")
                    .arg(driver->escapeIdentifier(colName,QSqlDriver::FieldName))
                    .arg(driver->formatValue(field));
            conditions.append(condition);
        }
    }

    mModel->setFilter(conditions.join(" and "));
}
