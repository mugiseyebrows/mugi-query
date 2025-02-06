#include "savedatadialog.h"
#include "ui_savedatadialog.h"

#include "model/dataimportcolumnmodel.h"
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QMessageBox>
#include "error.h"
#include "queryparser.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>
#include "widget/selectcolumnslistwidget.h"
#include "datastreamer.h"
#include "tokens.h"
#include "callonce.h"
#include <QTimer>
#include "splitterutil.h"
#include "settings.h"

/*static*/
DataSaveDialogState SaveDataDialog::mState = DataSaveDialogState();

QString unescapeIdentifier(const QString identifier) {
    if (identifier.startsWith("`") && identifier.endsWith("`")) {
        return identifier.mid(1,identifier.size()-2);
    }
    return identifier;
}

SaveDataDialog::SaveDataDialog(const QString& connectionName, QSqlQueryModel* model, const Tokens &tokens, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveDataDialog), mConnectionName(connectionName), mModel(model), mUpdatePreview(new CallOnce("onUpdatePreview",0,this))
{
    ui->setupUi(this);

    QStringList fields;
    for(int c=0;c<model->columnCount();c++) {
        fields << model->headerData(c,Qt::Horizontal).toString();
    }

    QStringList tables = QueryParser::tableNamesFromSelectQuery(model->query().lastQuery());

    QString table = tables.size() == 1 ? tables[0] : "query";

    mTable = table;

    ui->columns->setFields(fields);
    ui->columns->data()->setAllChecked();
    ui->columns->setLabelsMode(SelectColumnsWidget::LabelsModeLong);

    OutputType::initComboBox(ui->output);
    DataFormat::initComboBox(ui->format);

    ui->table->setText(table);
    // ui->outputName->setText(table + DataFormat::extension(format()));
    ui->preview->setTokens(tokens);

    connect(mUpdatePreview,SIGNAL(call()),this,SLOT(onUpdatePreview()));
    connect(ui->columns,SIGNAL(dataChanged(int,QModelIndex,QModelIndex)),mUpdatePreview,SLOT(onPost()));

    QTimer::singleShot(0,[=](){
        SplitterUtil::setRatio(ui->horizontalSplitter,{1,2});
        SplitterUtil::setRatio(ui->verticalSplitter,{3,1});
    });

    on_format_currentIndexChanged(ui->format->currentIndex());

    mUserName = false;
    connect(ui->outputName, &QLineEdit::textEdited, [=](){
        mUserName = true;
    });
}

SaveDataDialog::~SaveDataDialog()
{
    delete ui;
}

DataFormat::Format SaveDataDialog::format() const
{
    return DataFormat::value(ui->format);
}

QList<bool> SaveDataDialog::keysChecked() const
{
    return keysModel()->checkedAsBoolList();
}

QList<bool> SaveDataDialog::dataChecked() const
{
    return dataModel()->checkedAsBoolList();
}

QString SaveDataDialog::table() const
{
    return ui->table->text();
}

OutputType::Type SaveDataDialog::output() const
{
    return OutputType::value(ui->output);
}

void SaveDataDialog::accept()
{
    if (!dataModel()->hasAnyChecked()) {
        Error::show(this,"You need to check at least one data field");
        return;
    }
    if (format() == DataFormat::SqlUpdate) {
        if (!keysModel()->hasAnyChecked()) {
            Error::show(this,"You need to check at least one field for where clause");
            return;
        }
    }
    if (output() == OutputType::File) {
        if (ui->outputDir->text().isEmpty()) {
            Error::show(this,"Specify output directory");
            return;
        }
        if (ui->outputName->text().isEmpty()) {
            Error::show(this,"Specify output name");
            return;
        }
        if (QFile(filePath()).exists()) {
            QString text = QString("File %1 already exists, overwrite?").arg(ui->outputName->text());
            if (QMessageBox::question(this, "File exists", text, QMessageBox::Yes, QMessageBox::No) != QMessageBox::Yes) {
                return;
            }
        }
    }

    QDialog::accept();
}

DataImportColumnModel* SaveDataDialog::dataModel() const {
    return ui->columns->data()->model();
}

DataImportColumnModel* SaveDataDialog::keysModel() const {
    return ui->columns->keys()->model();
}

void SaveDataDialog::init()
{
    if (!mState.isValid()) {
        QString home = QDir(Settings::instance()->homePath()).filePath(mConnectionName);
        ui->outputDir->setText(QDir::toNativeSeparators(home));
        mUpdatePreview->onPost();
        return;
    }
    ui->output->setCurrentText(mState.outputType());
    ui->outputDir->setText(mState.outputDir());
    ui->format->setCurrentText(mState.dataFormat());
    mUpdatePreview->onPost();
}

void SaveDataDialog::save()
{
    mState.setOutputType(ui->output->currentText());
    mState.setOutputDir(ui->outputDir->text());
    mState.setDataFormat(ui->format->currentText());
}

QString SaveDataDialog::filePath() const
{
    return QDir(ui->outputDir->text()).filePath(ui->outputName->text());
}

void SaveDataDialog::on_table_textChanged(const QString& table)
{
    ui->columns->setTable(table);
    mUpdatePreview->onPost();
}

void SaveDataDialog::on_format_currentIndexChanged(int)
{
    DataFormat::Format format = this->format();
    ui->columns->setFormat(format);

    QString base;

    if (mUserName) {
        base = QFileInfo(ui->outputName->text()).completeBaseName();
    } else {
        if (format == DataFormat::SqlInsert) {
            base = "insert." + mTable;
        } else if (format == DataFormat::SqlUpdate) {
            base = "update." + mTable;
        } else {
            base = mTable;
        }
    }

    QString name = base + "." + DataFormat::suffix(format);
    ui->outputName->setText(name);
    mUpdatePreview->onPost();
}

void SaveDataDialog::on_output_currentIndexChanged(int)
{
    bool file = output() == OutputType::File;
    ui->FileGroup->setVisible(file);
}

void SaveDataDialog::on_selectPath_clicked()
{
    QString path = QDir(ui->outputDir->text()).filePath(ui->outputName->text());
    QString filter = DataFormat::fileDialogFilter(format());
    path = QFileDialog::getSaveFileName(this,QString(),path,filter,0,QFileDialog::DontConfirmOverwrite);
    if (path.isEmpty()) {
        return;
    }
    QFileInfo fileInfo(path);
    QString dir = QDir::toNativeSeparators(fileInfo.dir().absolutePath());
    QString name = fileInfo.fileName();
    ui->outputDir->setText(dir);
    ui->outputName->setText(name);
    mUserName = true;
}


void SaveDataDialog::onUpdatePreview() {

    QString text;

    if (!ui->columns->hasAnyChecked()) {
        ui->preview->setText(text);
        return;
    }

    QTextStream stream(&text);
    QString error;
    bool hasMore;

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    DataStreamer::stream(db, stream, mModel, format(), table(),
                         dataChecked(), keysChecked(),
                         DataFormat::ActionSave, true, &hasMore, locale(), error);

    Error::show(this, error);

    if (hasMore) {
        text = text + "...";
    }

    ui->preview->setText(text);
}

