#include "schema2relationdialog.h"
#include "ui_schema2relationdialog.h"

#include "schema2tablemodel.h"
#include "schema2relationmodel.h"

#include <QDebug>

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

#include <QSet>

static QString findCommon(const QStringList& names1, const QStringList& names2) {
#if 0
    for(int i=0;i<names1.size();i++) {
        for(int j=0;j<names2.size();j++) {
            if (names1[i] == names2[j]) {
                return names1[i];
            }
        }
    }
    return QString();
#endif
    QSet<QString> names1_ = names1.toSet();
    QSet<QString> names2_ = names2.toSet();
    QSet<QString> intersection = names1_.intersect(names2_);
    if (!intersection.isEmpty()) {
        return intersection.values()[0];
    }
    return QString();
}

static QStringList toLower(const QStringList& items) {
    QStringList res;
    for(const QString& item: items) {
        res.append(item.toLower());
    }
    return res;
}

static QString findColumn(const QStringList& columns, const QStringList& guesses) {

    QStringList columns_ = toLower(columns);
    QStringList guesses_ = toLower(guesses);
    for(const QString& guess: guesses_) {
        if (columns_.contains(guess)) {
            return columns[columns_.indexOf(guess)];
        }
    }
    return QString();
}

void Schema2RelationDialog::init(Schema2TableModel *childTable, Schema2TableModel *parentTable, Schema2RelationModel *model)
{
    if (!childTable || !parentTable) {
        qDebug() << "error" << childTable << parentTable << __FILE__ << __LINE__;
        return;
    }

    ui->childTable->setText(childTable->tableName());
    ui->parentTable->setText(parentTable->tableName());

    QStringList childColumns = childTable->newNames();
    QStringList parentColumns = parentTable->newNames();

    ui->childColumn->addItems(childColumns);
    ui->parentColumn->addItems(parentColumns);

    if (model) {
        setCurrentText(ui->childColumn, model->childColumn());
        setCurrentText(ui->parentColumn, model->parentColumn());
        ui->constraintName->setText(model->constraintName());
    } else {

        ui->remove->setEnabled(false);

        bool childFound = false;
        bool parentFound = false;


        QString parentTableName = parentTable->tableName();
        QString childTableName = childTable->tableName();

        QString constraintName = QString("FK_%1_%2").arg(parentTableName).arg(childTableName);
        ui->constraintName->setText(constraintName);

        QString guessChild = findColumn(childColumns, {parentTableName + "_ID", parentTableName + "_OBJID"});

        if (!guessChild.isEmpty()) {
            childFound = true;
            setCurrentText(ui->childColumn, guessChild);
            QString guessParent = findColumn(parentColumns, {guessChild, "ID", "OBJID"});
            if (!guessParent.isEmpty()) {
                setCurrentText(ui->parentColumn, guessParent);
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

QString Schema2RelationDialog::constraintName() const {
    return ui->constraintName->text();
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
