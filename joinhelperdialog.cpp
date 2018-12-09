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
#include "rowvaluegetter.h"
#include <vector>
#include <QDebug>
#include "relation.h"

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

    connect(tablesModel,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this,SLOT(onTableModelDataChanged(QModelIndex,QModelIndex,QVector<int>)));
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



void JoinHelperDialog::findPath()
{

    CheckableStringListModel* tablesModel = qobject_cast<CheckableStringListModel*>(ui->tables->model());

    QAbstractItemModel* model = ui->relations->model();

    if (!tablesModel || !model) {
        return;
    }

    QStringList checked = tablesModel->checked();
    if (checked.size() < 2) {
        ui->query->setText("check two or more tables");
        return;
    }

    QList<Relation> relations;
    for(int row=0;row<model->rowCount();row++) {
        RowValueGetter g(model,row);
        Relation relation(g(0).toString(),g(1).toString());
        if (!relation.isEmpty()) {
            relations.append(relation);
        }
    }
    QStringList tables;
    foreach(const Relation& relation, relations) {
        tables << relation.primaryTable() << relation.foreignTable();
    }
    tables = tables.toSet().toList();
    QStringList errors;
    foreach(const QString& table, checked) {
        if (!tables.contains(table)) {
            errors << QString("no relations specified for %1").arg(table);
        }
    }
    for(int i=0;i<relations.size();i++) {
        Relation& relation = relations[i];
        relation.setPrimary(tables.indexOf(relation.primaryTable()));
        relation.setForeign(tables.indexOf(relation.foreignTable()));
    }

    if (errors.size() > 0) {
        ui->query->setText(errors.join("\n"));
        return;
    }

    QMap<int, QList<int> > links;
    foreach(const Relation& relation, relations) {
        int primary = tables.indexOf(relation.primaryTable());
        int foreign = tables.indexOf(relation.foreignTable());
        if (!links.contains(primary)) {
            links[primary] = QList<int>();
        }
        if (!links.contains(foreign)) {
            links[foreign] = QList<int>();
        }
        if (primary == foreign) {
            errors << QString("recursive relations are not supported %1.%2 %3.%4")
                      .arg(relation.primaryTable())
                      .arg(relation.primaryKey())
                      .arg(relation.foreignTable())
                      .arg(relation.foreignKey());
        } else {
            links[primary].append(foreign);
            links[foreign].append(primary);
        }
    }

    if (!errors.isEmpty()) {
        ui->query->setText(errors.join("\n"));
    }

    int origin = tables.indexOf(checked[0]);
    QList<QList<int> > paths;
    QList<QList<int> > paths_;
    QList<int> initial;
    initial << origin;
    paths.append(initial);
    QList<QList<int> > complete;
    int iter = 0;
    while(!paths.isEmpty()) {
        iter++;
        for(int i=0;i<paths.size();i++) {
            QList<int> path = paths[i];
            int head = path[path.size()-1];
            QList<int> possibilities = links[head];
            bool stuck = true;
            foreach(int possibility, possibilities) {
                if (!path.contains(possibility)) {
                    QList<int> newPath = path;
                    newPath << possibility;
                    paths_ << newPath;
                    stuck = false;
                }
            }
            if (stuck) {
                complete << path;
            }
        }
        if (iter > 100) {
            qDebug() << "";
        }

        paths = paths_;
        paths_ = QList<QList<int> >();
    }

    QList<int> checked_;
    foreach(const QString& table, checked) {
        checked_ << tables.indexOf(table);
    }

    qDebug() << checked_;

    QList<QList<int> > results;

    foreach(const QList<int>& path, complete) {
        QList<int> indexes;
        foreach(int table, checked_) {
            int index = path.indexOf(table);
            if (index < 0) {
                break;
            }
            indexes << index;
        }

        if (checked_.size() > indexes.size()) {
            continue;
        }

        int begin = *std::min_element(indexes.begin(),indexes.end());
        int end = *std::max_element(indexes.begin(),indexes.end());
        //std::vector<int> res;
        //std::copy(begin,end+1,std::back_inserter(res));
        //results << QList<int>::fromVector(QVector<int>::fromStdVector(res));

        results << path.mid(begin,end-begin+1);
    }

    if (results.isEmpty()) {
        ui->query->setText("no path found");
        return;
    }

    bool leftJoin = false;

    QList<int> result = results[0];

    if (!leftJoin) {
        QStringList tables__;
        QStringList relations__;
        foreach(int table, result) {
            tables__ << tables[table];
        }
        for(int i=1;i<result.size();i++) {
            int table1 = result[i-1];
            int table2 = result[i];
            foreach(const Relation& relation, relations) {
                bool t1 = (relation.primary() == table1 &&
                           relation.foreign() == table2);
                bool t2 = (relation.primary() == table2 &&
                           relation.foreign() == table1);
                if (t1 || t2) {
                    if (t1) {
                        relations__ << QString("%1.%2=%3.%4")
                                       .arg(tables[table1])
                                       .arg(relation.primaryKey())
                                       .arg(tables[table2])
                                       .arg(relation.foreignKey());
                    } else if (t2) {
                        relations__ << QString("%1.%2=%3.%4")
                                       .arg(tables[table2])
                                       .arg(relation.primaryKey())
                                       .arg(tables[table1])
                                       .arg(relation.foreignKey());
                    }
                }
            }
        }

        QString query = QString("select * from %1\nwhere %2")
                .arg(tables__.join(", "))
                .arg(relations__.join("\nand "));

        ui->query->setText(query);
    }


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

void JoinHelperDialog::onTableModelDataChanged(QModelIndex,QModelIndex,QVector<int>) {
    findPath();
}
