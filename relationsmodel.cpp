#include "relationsmodel.h"

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QRegularExpression>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

RelationsModel::RelationsModel(QObject *parent) : QStandardItemModel(0,2,parent), mChanged(false)
{
    setHeaderData(0,Qt::Horizontal,"Foreign key");
    setHeaderData(1,Qt::Horizontal,"Primary key");
}

RelationsModel::~RelationsModel()
{

}

bool RelationsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    mChanged = true;
    return QStandardItemModel::setData(index,value,role);
}

bool RelationsModel::changed() const
{
    return mChanged;
}

void RelationsModel::load(const QString &path)
{
    if (rowCount() > 0) {
        removeRows(0,rowCount());
    }

    QFile file(path);
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream stream(&file);
            stream.setCodec(QTextCodec::codecForName("UTF-8"));
            while (!stream.atEnd()) {
                QString line = stream.readLine();
                QStringList cols = line.split(QRegularExpression("\t"));
                if (cols.size() > 1) {
                    int row = rowCount();
                    insertRow(row);
                    setData(index(row,0),cols[0].trimmed());
                    setData(index(row,1),cols[1].trimmed());
                    row++;
                }
            }
            file.close();
        }
    }

    mChanged = false;

}

void RelationsModel::save(const QString &path)
{

    if (!mChanged) {
        return;
    }

    qDebug() << "RelationsModel::save" << path;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(qApp->activeWindow(),"Error",QString("Cannot open file %1").arg(path));
        return;
    }
    QTextStream stream(&file);
    stream.setCodec(QTextCodec::codecForName("UTF-8"));
    for(int row=0;row<rowCount();row++) {
        QString col1 = data(index(row,0)).toString().trimmed();
        QString col2 = data(index(row,1)).toString().trimmed();
        if (!col1.isEmpty() && !col2.isEmpty()) {
            stream << col1 << "\t" << col2 << "\n";
        }
    }
    stream.flush();
    file.close();

    mChanged = false;
}
