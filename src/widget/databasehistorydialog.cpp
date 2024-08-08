#include "databasehistorydialog.h"
#include "ui_databasehistorydialog.h"

#include <QSqlQueryModel>
#include <QVariant>
#include <QSqlQuery>

#include "richheaderview/richheaderview.h"
#include "datetimerangewidget.h"
#include "datetimerangewidgetmanager.h"
#include <QComboBox>
#include "intlineedit.h"
#include "query_exec.h"
#include "history.h"

static QStringList availableSqlDrivers() {
    QStringList drivers = QSqlDatabase::drivers();
    drivers.removeAll("QMYSQL3");
    drivers.removeAll("QOCI8");
    drivers.removeAll("QODBC3");
    drivers.removeAll("QPSQL7");
    drivers.removeAll("QTDS7");
    return drivers;
}

DatabaseHistoryDialog::DatabaseHistoryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseHistoryDialog)
{
    ui->setupUi(this);

    History::instance();

    RichHeaderView* view = new RichHeaderView(Qt::Horizontal);
    //view->setSec
    RichHeaderData data = view->data();
    data.subsectionSizes({ui->tableView->verticalHeader()->defaultSectionSize() * 3 / 2});
    data.stretchFixed(false);

    QWidget* viewport = view->viewport();
    mDateEdit = new DateTimeRangeWidget(viewport);
    mConnectionNameEdit = new QLineEdit(viewport);
    mDriverEdit = new QComboBox(viewport);
    mHostEdit = new QLineEdit(viewport);
    mUserEdit = new QLineEdit(viewport);
    mDatabaseEdit = new QLineEdit(viewport);
    mPortEdit = new IntLineEdit(viewport);

    mDriverEdit->addItem("any");
    mDriverEdit->addItems(availableSqlDrivers());
    mDriverEdit->setCurrentIndex(0);

    DateTimeRangeWidgetManager* manager = new DateTimeRangeWidgetManager(this);
    manager->init(mDateEdit);
    manager->set365days();

    connect(mDateEdit,SIGNAL(dateTimesChanged(QDateTime,QDateTime)),this,SLOT(onUpdateQuery()));
    QList<QLineEdit*> lineEdits = {mConnectionNameEdit, mHostEdit, mUserEdit, mDatabaseEdit, mPortEdit};
    foreach (QLineEdit* edit, lineEdits) {
        connect(edit,SIGNAL(textChanged(QString)),this,SLOT(onUpdateQuery()));
    }
    connect(mDriverEdit,SIGNAL(currentIndexChanged(int)),this,SLOT(onUpdateQuery()));

    data.cell(0,col_date).widget(mDateEdit);
    data.cell(0,col_connectionName).widget(mConnectionNameEdit);
    data.cell(0,col_driver).widget(mDriverEdit);
    data.cell(0,col_host).widget(mHostEdit);
    data.cell(0,col_user).widget(mUserEdit);
    //data.cell(0,col_password).widget();
    data.cell(0,col_database).widget(mDatabaseEdit);
    data.cell(0,col_port).widget(mPortEdit);

    QSqlQueryModel* model = new QSqlQueryModel(this);
    ui->tableView->setModel(model);

    ui->tableView->setHorizontalHeader(view);

    onUpdateQuery();
}

DatabaseHistoryDialog::~DatabaseHistoryDialog()
{
    delete ui;
}

QString likeExpr(const QString& value) {
    return value.isEmpty() ? "%" : "%" + value + "%";
}

static QString like(const QString& value) {
    if (value.isEmpty()) {
        return QString();
    }
    return "%" + value + "%";
}

void DatabaseHistoryDialog::onUpdateQuery() {

    QSqlDatabase db = QSqlDatabase::database("_history");

    QDateTime date1 = mDateEdit->dateTime1();
    QDateTime date2 = mDateEdit->dateTime2();
    QString connectionName = like(mConnectionNameEdit->text());
    QString driver = mDriverEdit->currentText();
    QString host = like(mHostEdit->text());
    QString user = like(mUserEdit->text());
    QString database = like(mDatabaseEdit->text());

    QSqlQuery q(db);

    QStringList conditions;
    QVariantList values;
    conditions.append("date between ? and ?");
    values.append(date1);
    values.append(date2);
    if (driver != "any") {
        conditions.append("driver=?");
        values.append(driver);
    }
    if (!connectionName.isEmpty()) {
        conditions.append("connectionName like ?");
        values.append(connectionName);
    }
    if (!host.isEmpty()) {
        conditions.append("host like ?");
        values.append(host);
    }
    if (!user.isEmpty()) {
        conditions.append("user like ?");
        values.append(user);
    }
    if (!database.isEmpty()) {
        conditions.append("database like ?");
        values.append(database);
    }

    QString expr = "select * from database where " + conditions.join(" and ") + " order by date desc";
    q.prepare(expr);
    for(const QVariant& value: values) {
        q.addBindValue(value);
    }
    QUERY_EXEC(q);
    QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(ui->tableView->model());
    if (!model) {
        qDebug() << "!model";
        return;
    }
    model->setQuery(q);
    ui->tableView->setColumnWidth(0, 160);
}

void DatabaseHistoryDialog::select(const QString &connectionName)
{
    QAbstractItemModel* m = ui->tableView->model();
    for(int row=0;row<m->rowCount();row++) {
        if (m->data(m->index(row,col_connectionName)).toString() == connectionName) {
            ui->tableView->setCurrentIndex(m->index(row,0));
            on_tableView_doubleClicked(QModelIndex());
            return;
        }
    }
}

QVariant DatabaseHistoryDialog::data(int column) const
{
    QAbstractItemModel* m = ui->tableView->model();
    return m->data(m->index(ui->tableView->currentIndex().row(),column));
}

QString DatabaseHistoryDialog::dataToString(int column) const
{
    return data(column).toString();
}

int DatabaseHistoryDialog::dataToInt(int column) const
{
    return data(column).toInt();
}

void DatabaseHistoryDialog::on_tableView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    QDialog::accept();
}

QString DatabaseHistoryDialog::connectionName() const {
    return dataToString(col_connectionName);
}

QString DatabaseHistoryDialog::driver() const {
    return dataToString(col_driver);
}

QString DatabaseHistoryDialog::host() const
{
    return dataToString(col_host);
}

QString DatabaseHistoryDialog::user() const
{
    return dataToString(col_user);
}

QString DatabaseHistoryDialog::password() const
{
    return dataToString(col_password);
}

QString DatabaseHistoryDialog::database_() const
{
    return dataToString(col_database);
}

int DatabaseHistoryDialog::port() const
{
    return dataToInt(col_port);
}

