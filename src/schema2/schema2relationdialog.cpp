#include "schema2relationdialog.h"
#include "ui_schema2relationdialog.h"

#include "schema2tablemodel.h"
#include "schema2relationmodel.h"

Schema2RelationDialog::Schema2RelationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Schema2RelationDialog),
    mRemove(false)
{
    ui->setupUi(this);
}

Schema2RelationDialog::~Schema2RelationDialog()
{
    delete ui;
}

static void setCurrentText(QComboBox* combo, const QString& text) {
    for(int i=0;i<combo->count();i++) {
        if (combo->itemText(i) == text) {
            combo->setCurrentIndex(i);
            return;
        }
    }
}

static QString findCommon(const QStringList& names1, const QStringList& names2) {
    for(int i=0;i<names1.size();i++) {
        for(int j=0;j<names2.size();j++) {
            if (names1[i] == names2[j]) {
                return names1[i];
            }
        }
    }
    return QString();
}

void Schema2RelationDialog::init(Schema2TableModel *childTable, Schema2TableModel *parentTable, Schema2RelationModel *model)
{
    ui->childTable->setText(childTable->tableName());
    ui->parentTable->setText(parentTable->tableName());

    QStringList childColumns = childTable->newNames();
    QStringList parentColumns = parentTable->newNames();

    ui->childColumn->addItems(childColumns);
    ui->parentColumn->addItems(parentColumns);

    if (model) {
        setCurrentText(ui->childColumn, model->childColumn());
        setCurrentText(ui->parentColumn, model->parentColumn());
    } else {

        bool childFound = false;
        bool parentFound = false;

        QString guess = parentTable->tableName() + "_id";
        if (childColumns.contains(guess)) {
            childFound = true;
            setCurrentText(ui->childColumn, guess);
            if (parentColumns.contains(guess)) {
                setCurrentText(ui->parentColumn, guess);
                parentFound = true;
            } else if (parentColumns.contains("id")) {
                setCurrentText(ui->parentColumn, "id");
                parentFound = true;
            }
        } else {
            QString common = findCommon(childColumns, parentColumns);
            if (!common.isEmpty()) {
                setCurrentText(ui->childColumn, common);
                setCurrentText(ui->parentColumn, common);
                childFound = true;
                parentFound = true;
            }
        }
        if (!childFound || !parentFound) {
            // todo implement find related columns by type
            // todo implement find parent column by PK
        }

    }

    setWindowTitle(model ? "Edit Relation" : "Create Relation");
}

bool Schema2RelationDialog::remove() const
{
    return mRemove;
}

QString Schema2RelationDialog::childColumn() const
{
    return ui->childColumn->currentText();
}

QString Schema2RelationDialog::parentColumn() const
{
    return ui->parentColumn->currentText();
}

bool Schema2RelationDialog::constrained() const
{
    return ui->constrained->isChecked();
}

void Schema2RelationDialog::on_remove_clicked()
{
    mRemove = true;
    accept();
}
