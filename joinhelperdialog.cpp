#include "joinhelperdialog.h"
#include "ui_joinhelperdialog.h"

#include <QDir>
#include "settings.h"
#include <QSqlDatabase>
#include <QStandardItemModel>
#include <QTextStream>
#include <QTextCodec>
#include <QMessageBox>
#include "modelappender.h"
#include "itemdelegatewithcompleter.h"
#include "checkablestringlistmodel.h"

JoinHelperDialog::JoinHelperDialog(const QString& connectionName,
                                   const Tokens& tokens, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinHelperDialog),
    mConnectionName(connectionName)
{
    ui->setupUi(this);
    ui->query->setTokens(tokens);

    QStandardItemModel* model = new QStandardItemModel(0,2,this);

    model->setHeaderData(0,Qt::Horizontal,"Foreign key");
    model->setHeaderData(1,Qt::Horizontal,"Primary key");

    QString path = filePath();
    QFile file(path);
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream stream(&file);
            stream.setCodec(QTextCodec::codecForName("UTF-8"));
            while (!stream.atEnd()) {
                QString line = stream.readLine();
                QStringList cols = line.split(QRegularExpression("\t"));
                if (cols.size() > 1) {
                    int row = model->rowCount();
                    model->insertRow(row);
                    model->setData(model->index(row,0),cols[0].trimmed());
                    model->setData(model->index(row,1),cols[1].trimmed());
                    row++;
                }
            }
            file.close();
        }
    }

    model->insertRow(model->rowCount());

    ui->relations->setModel(model);

    ItemDelegateWithCompleter* itemDelegate = new ItemDelegateWithCompleter(tokens.fields(true),this);
    ui->relations->setItemDelegate(itemDelegate);

    ModelAppender* appender = new ModelAppender(this);
    appender->setModel(model);

    CheckableStringListModel* tablesModel = new CheckableStringListModel(tokens.tables(),this);
    ui->tables->setModel(tablesModel);
}

JoinHelperDialog::~JoinHelperDialog()
{
    delete ui;
}

QString JoinHelperDialog::filePath() const
{
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList props;
    props << "relations" << db.driverName() << db.hostName()
          << db.userName() << db.databaseName();

    QString name = props.join(" ")
            .replace(QRegExp("[^a-z0-9._ -]",Qt::CaseInsensitive)," ")
            .replace(QRegExp("[ ]+")," ") + ".txt";

    return QDir(Settings::instance()->dir()).filePath(name);
}


void JoinHelperDialog::accept()
{
    QString path = filePath();

    // todo model.isChanged()
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this,"Error",QString("Cannot open file %1").arg(path));
        return;
    }
    QTextStream stream(&file);
    stream.setCodec(QTextCodec::codecForName("UTF-8"));
    QAbstractItemModel* model = ui->relations->model();

    for(int row=0;row<model->rowCount();row++) {
        QString col1 = model->data(model->index(row,0)).toString().trimmed();
        QString col2 = model->data(model->index(row,1)).toString().trimmed();
        if (!col1.isEmpty() && !col2.isEmpty()) {
            stream << col1 << "\t" << col2 << "\n";
        }
    }
    stream.flush();
    file.close();

    QDialog::accept();
}
