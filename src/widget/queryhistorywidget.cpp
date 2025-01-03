#include "queryhistorywidget.h"
#include "ui_queryhistorywidget.h"

#include <QSqlQueryModel>
#include <QStringListModel>
#include <QDebug>
#include "copyeventfilter.h"
#include "model/queryhistorymodel.h"
#include <QClipboard>
#include "richheaderview/richheaderview.h"
#include "widget/datetimerangewidget.h"

#include <QTimer>
#include "datetimerangewidgetmanager.h"
#include "callonce.h"
#include <QLineEdit>
#include <QComboBox>
#include <algorithm>
#include "query_exec.h"
#include "daterangewidget.h"
#include "uniq.h"

static QList<int> partialySelectedRows(QItemSelectionModel* selectionModel) {
    QModelIndexList indexes = selectionModel->selectedIndexes();
    QList<int> rows;
    foreach (const QModelIndex& index, indexes) {
        rows << index.row();
    }
    rows = uniq(rows);
    std::sort(rows.begin(),rows.end());
    return rows;
}


QueryHistoryWidget::QueryHistoryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryHistoryWidget)
{
    ui->setupUi(this);

    QueryHistoryModel* model = new QueryHistoryModel(this);
    ui->tableView->setModel(model);

    ui->tableView->setColumnWidth(0,160);

    CopyEventFilter* filter = new CopyEventFilter(this);
    filter->setView(ui->tableView);

    connect(filter,&CopyEventFilter::copy,[=](){
        QStringList queries = selectedQueries();
        if (queries.isEmpty()) {
            return;
        }
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(queries.join(";\n"));
    });

    RichHeaderView* view = new RichHeaderView(Qt::Horizontal, ui->tableView);

    RichHeaderData data = view->data();

    data.stretchFixed(false);

    data.subsectionSizes({ui->tableView->verticalHeader()->defaultSectionSize() * 3 / 2});

    //DateTimeRangeWidget* dateEdit = new DateTimeRangeWidget(view->viewport());

    DateRangeWidget* dateEdit = new DateRangeWidget(view->viewport());

    QLineEdit* queryEdit = new QLineEdit(view->viewport());
    QComboBox* connectionNameEdit = new QComboBox(view->viewport());

    CallOnce* once = new CallOnce("updateQuery", 500);
    connect(queryEdit,SIGNAL(textChanged(QString)),once,SLOT(onPost()));
    connect(once,SIGNAL(call()),this,SLOT(onUpdateQuery()));

    connect(dateEdit,SIGNAL(changed(int,int,QDate,QDate)),this,SLOT(onUpdateQuery()));
    connect(connectionNameEdit,SIGNAL(currentIndexChanged(int)),this,SLOT(onUpdateQuery()));

    /*DateTimeRangeWidgetManager* manager = new DateTimeRangeWidgetManager(this);
    manager->init(dateEdit);*/

    data.cell(0, QueryHistoryModel::col_date).widget(dateEdit).padding(0,5);
    data.cell(0, QueryHistoryModel::col_query).widget(queryEdit);
    data.cell(0, QueryHistoryModel::col_connectionName).widget(connectionNameEdit);

    ui->tableView->setHorizontalHeader(view);

    view->setStretchLastSection(true);

}

QWidget* QueryHistoryWidget::edit(int column) {
    RichHeaderView* view = headerView();
    if (!view) {
        return 0;
    }
    RichHeaderData data = view->data();
    return data.cell(0, column).widget();
}

DateRangeWidget* QueryHistoryWidget::dateEdit() {
    return qobject_cast<DateRangeWidget*>(edit(QueryHistoryModel::col_date));
}

QComboBox* QueryHistoryWidget::connectionNameEdit() {
    return qobject_cast<QComboBox*>(edit(QueryHistoryModel::col_connectionName));
}

QLineEdit* QueryHistoryWidget::queryEdit() {
    return qobject_cast<QLineEdit*>(edit(QueryHistoryModel::col_query));
}

RichHeaderView* QueryHistoryWidget::headerView() {
    return qobject_cast<RichHeaderView*>(ui->tableView->horizontalHeader());
}

QueryHistoryWidget::~QueryHistoryWidget()
{
    delete ui;
}

QStringList QueryHistoryWidget::selectedQueries() const {
    QList<int> rows = partialySelectedRows(ui->tableView->selectionModel());
    QStringList queries;
    QAbstractItemModel* model = ui->tableView->model();
    foreach (int row, rows) {
        queries << model->data(model->index(row,QueryHistoryModel::col_query)).toString();
    }
    return queries;
}

void QueryHistoryWidget::on_tableView_doubleClicked(QModelIndex index) {
    QAbstractItemModel* m = ui->tableView->model();
    QString connectionName = m->data(m->index(index.row(),QueryHistoryModel::col_connectionName)).toString();
    QString query = m->data(m->index(index.row(),QueryHistoryModel::col_query)).toString();
    emit appendQuery(connectionName,query);
}

void QueryHistoryWidget::refresh(const QString& connectionName)
{
    QComboBox* comboBox = connectionNameEdit();
    if (!comboBox) {
        return;
    }
    QSqlDatabase db = QSqlDatabase::database("_history");
    QSqlQuery q(db);
    q.prepare("select distinct connectionName from query");
    QUERY_EXEC(q);
    QStringList connectionNames = {"any"};
    while (q.next()) {
        connectionNames.append(q.value(0).toString());
    }
    comboBox->setModel(new QStringListModel(connectionNames, this));
    int index = connectionNames.indexOf(connectionName);
    if (index > -1) {
        comboBox->setCurrentIndex(index);
    }
    onUpdateQuery();
}

void QueryHistoryWidget::on_refresh_clicked()
{
    refresh(connectionNameEdit()->currentText());
}

void QueryHistoryWidget::on_copy_clicked()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QStringList queries = selectedQueries();
    if (queries.isEmpty()) {
        return;
    }
    QAbstractItemModel* model = ui->tableView->model();
    QString connectionName = model->data(model->index(index.row(),QueryHistoryModel::col_connectionName)).toString();
    emit appendQuery(connectionName,queries.join(";\n"));
}

static QString like(const QString& value) {
    if (value.isEmpty()) {
        return QString();
    }
    return "%" + value + "%";
}

#include <QFontMetrics>

void QueryHistoryWidget::onUpdateQuery() {

    if (!headerView()) {
        return;
    }

    QSqlDatabase db = QSqlDatabase::database("_history");
    QString query = like(queryEdit()->text());

    DateRangeWidget* dateEdit = this->dateEdit();


    DateRangeWidget::Mode mode = dateEdit->mode();

    QString connectionName = connectionNameEdit()->currentText();

    QStringList conditions;
    QVariantList values;

    if (mode == DateRangeWidget::All) {
        // do nothing
        //qDebug() << "All";
    } else if (mode == DateRangeWidget::NDays) {
        QDate date = QDate::currentDate().addDays(-dateEdit->days());
        conditions.append("date(date) > ?");
        values.append(date);
        //qDebug() << "NDays" << date;
    } else if (mode == DateRangeWidget::Range) {
        conditions.append("date(date) between ? and ?");
        values.append(dateEdit->date1());
        values.append(dateEdit->date2());
        //qDebug() << "Range" << dateEdit->date1() << dateEdit->date2();
    }

    if (connectionName != "any") {
        conditions.append("connectionName=?");
        values.append(connectionName);
    }
    if (!query.isEmpty()) {
        conditions.append("query like ?");
        values.append(query);
    }

    QSqlQuery q(db);
    q.prepare("select * from query where " + conditions.join(" and ") + " order by date desc");
    for(const QVariant& value: values) {
        q.addBindValue(value);
    }
    QUERY_EXEC(q);
    QSqlQueryModel* model = qobject_cast<QSqlQueryModel*>(ui->tableView->model());
    if (!model) {
        qDebug() << "not model";
        return;
    }
    model->setQuery(q);

    QFontMetrics m(dateEdit->font());
    int width = m.horizontalAdvance("0000-00-00") * 3;
    ui->tableView->setColumnWidth(QueryHistoryModel::col_date, width);
}
