#include "dataimportmodel.h"

#include "sqldatatypes.h"
#include <QColor>
#include <QDebug>

#include <QRegularExpression>
#include <QDate>

static QVariant parseInt(const QVariant& value, bool* ok) {
    if (value.typeId() == QMetaType::Int) {
        *ok = true;
        return value;
    }
    QString s = value.toString();
    if (s.isEmpty()) {
        *ok = false;
        return QVariant();
    }
    return s.toInt(ok);
}
static QVariant parseDouble(const QVariant& value, bool* ok) {
    if (value.typeId() == QMetaType::Double) {
        *ok = true;
        return value;
    }
    QString s = value.toString();
    if (s.isEmpty()) {
        *ok = false;
        return QVariant();
    }
    return s.replace(",",".").toDouble(ok);
}

static QVariant parseDate(const QVariant& value, bool* ok) {
    if (value.typeId() == QMetaType::QDate) {
        *ok = true;
        return value;
    }
    QRegularExpression rx("^([0-9]{4})-([0-9]{2})-([0-9]{2})$");
    auto m = rx.match(value.toString());
    if (m.hasMatch()) {
        int year = m.captured(1).toInt();
        int month = m.captured(2).toInt();
        int day = m.captured(3).toInt();
        *ok = true;
        return QDate(year, month, day);
    }
    rx = QRegularExpression("^([0-9]{2})-([0-9]{2})-([0-9]{4})$");
    m = rx.match(value.toString());
    if (m.hasMatch()) {
        int day = m.captured(1).toInt();
        int month = m.captured(2).toInt();
        int year = m.captured(3).toInt();
        *ok = true;
        return QDate(year, month, day);
    }
    rx = QRegularExpression("^([0-9]{2})[.]([0-9]{2})[.]([0-9]{4})$");
    m = rx.match(value.toString());
    if (m.hasMatch()) {
        int day = m.captured(1).toInt();
        int month = m.captured(2).toInt();
        int year = m.captured(3).toInt();
        *ok = true;
        return QDate(year, month, day);
    }
    rx = QRegularExpression("^([0-9]{2})[.]([0-9]{2})[.]([0-9]{2})$");
    m = rx.match(value.toString());
    if (m.hasMatch()) {
        int day = m.captured(1).toInt();
        int month = m.captured(2).toInt();
        int year = m.captured(3).toInt();
        *ok = true;
        // year 1970..2069
        if (year > 70) {
            year = 1900 + year;
        } else {
            year = 2000 + year;
        }
        return QDate(year, month, day);
    }
    *ok = false;
    return QVariant();
}
static QVariant parseDateTime(const QVariant& value, bool* ok) {
    QRegularExpression rx("^([0-9]+)-([0-9]+)-([0-9]+).([0-9:]+)([.][0-9]+)?$");
    auto match = rx.match(value.toString());
    if (match.hasMatch()) {
        int year = match.captured(1).toInt();
        int month = match.captured(2).toInt();
        int day = match.captured(3).toInt();
        int ms = 0;
        int h = 0;
        int m = 0;
        int s = 0;
        if (match.lastCapturedIndex() == 5) {
            ms = match.captured(5).mid(1).toInt();
        }
        QStringList hms = match.captured(4).split(":");
        if (hms.size() > 0) {
            h = hms[0].toInt(nullptr, 10);
        }
        if (hms.size() > 1) {
            m = hms[1].toInt(nullptr, 10);
        }
        if (hms.size() > 2) {
            s = hms[2].toInt(nullptr, 10);
        }
        *ok = true;
        return QDateTime(QDate(year, month, day), QTime(h, m, s, ms));
    }
    *ok = false;
    return QVariant();
}

QVariant DataImportModel::parsed(int row, int column)
{
    QMetaType::Type type = mTypes.value(column, QMetaType::UnknownType);

    QVariant value = data(index(row, column));
    bool ok;
    switch(type) {
    case QMetaType::Int:
        return parseInt(value, &ok);
    case QMetaType::Double:
        return parseDouble(value, &ok);
    case QMetaType::QDate:
        return parseDate(value, &ok);
    case QMetaType::QDateTime:
        return parseDateTime(value, &ok);
    case QMetaType::QString:
        return value;
    case QMetaType::UnknownType:
        return value;
    default:
        qDebug() << "DataImportModel::parsed not implemented for type" << type;
    }
    return value;
}

DataImportModel::DataImportModel(QObject *parent)
    : QStandardItemModel(5,5,parent)
{

}

QVariant DataImportModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ForegroundRole) {
        auto type = mTypes.value(index.column(), QMetaType::UnknownType);
        if (type == QMetaType::UnknownType || type == QMetaType::QString) {
            return QVariant();
        }
        bool ok = true;
        QVariant value = data(index);
        switch(type) {
        case QMetaType::Int:
            parseInt(value, &ok);
            break;
        case QMetaType::Double:
            parseDouble(value, &ok);
            break;
        case QMetaType::QDate:
            parseDate(value, &ok);
            break;
        case QMetaType::QDateTime:
            parseDateTime(value, &ok);
            break;
        case QMetaType::QString:
            break;
        case QMetaType::UnknownType:
            break;
        default:
            qDebug() << "data Qt::ForegroundRole not implemented for type" << type;
        }
        if (!ok) {
            return QVariant(QColor(Qt::red));
        }
    }
    return QStandardItemModel::data(index, role);
}


void DataImportModel::setTypes(const QMap<int, QMetaType::Type> &types, const QMap<int, int> &sizes)
{
    if (mTypes == types && mSizes == sizes) {
        //qDebug() << "same types";
        return;
    }
    mTypes = types;
    mSizes = sizes;

    emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
}

void DataImportModel::setLocale(const QLocale &locale)
{
    mLocale = locale;
}

void DataImportModel::setType(int column, QMetaType::Type type)
{
    mTypes[column] = type;
}

#include <QSqlRecord>

QSqlRecord DataImportModel::record(int row, bool *ok)
{

}
