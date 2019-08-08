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

/*static*/
DataSaveDialogState SaveDataDialog::mState = DataSaveDialogState();

QString unescapeIdentifier(const QString identifier) {
    if (identifier.startsWith("`") && identifier.endsWith("`")) {
        return identifier.mid(1,identifier.size()-2);
    }
    return identifier;
}

SaveDataDialog::SaveDataDialog(const QSqlDatabase& database, QSqlQueryModel* model, const Tokens &tokens, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveDataDialog), mDatabase(database), mModel(model), mUpdatePreview(new CallOnce("onUpdatePreview",0,this))
{
    ui->setupUi(this);

    QStringList fields;
    for(int c=0;c<model->columnCount();c++) {
        fields << model->headerData(c,Qt::Horizontal).toString();
    }

    bool many;
    QString table = QueryParser::tableNameFromSelectQuery(model->query().lastQuery(), &many);
    if (table.isEmpty()) {
        table = "query";
    }

    ui->columns->setFields(fields);
    ui->columns->data()->setAllChecked();
    ui->columns->setLabelsMode(SelectColumnsWidget::LabelsModeLong);

    OutputType::initComboBox(ui->output);
    DataFormat::initComboBox(ui->format);

    ui->table->setText(table);
    ui->outputName->setText(table + DataFormat::extension(format()));
    ui->preview->setTokens(tokens);

    connect(mUpdatePreview,SIGNAL(call()),this,SLOT(onUpdatePreview()));
    connect(ui->columns,SIGNAL(dataChanged(int,QModelIndex,QModelIndex)),mUpdatePreview,SLOT(onPost()));

    QTimer::singleShot(0,[=](){
        SplitterUtil::setRatio(ui->horizontalSplitter,{1,2});
        SplitterUtil::setRatio(ui->verticalSplitter,{3,1});
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
        QString home = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
        ui->outputDir->setText(home);
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
    QRegularExpression rx(DataFormat::extensionsRegExp(), QRegularExpression::CaseInsensitiveOption);
    QString text = ui->outputName->text();
    text.replace(rx, DataFormat::extension(format));
    ui->outputName->setText(text);
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
    DataStreamer::stream(mDatabase, stream, mModel, format(), table(),
                         dataChecked(), keysChecked(),
                         DataFormat::ActionSave, true, &hasMore, locale(), error);

    Error::show(this, error);

    if (hasMore) {
        text = text + "...";
    }

    ui->preview->setText(text);
}

