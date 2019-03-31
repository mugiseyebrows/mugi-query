#include "datautils.h"
#include "qisnumerictype.h"

#include <QAbstractItemModel>

QPolygonF DataUtils::toPolygon(const QList<QPair<QVariant,QVariant> >& data) {
    QPolygonF result;
    for(int i=0;i<data.size();i++) {
        result.append(QPointF(data[i].first.toDouble(),data[i].second.toDouble()));
    }
    return result;
}

#if 0
QList<QPair<QVariant,QVariant> > DataUtils::filterNull(const QList<QPair<QVariant,QVariant> >& data) {
    QList<QPair<QVariant,QVariant> > result;
    for(int i=0;i<data.size();i++) {
        if (data[i].first.isNull() || data[i].second.isNull()) {
            continue;
        }
        result.append(data[i]);
    }
    return result;
}
#endif



QList<QPair<QVariant,QVariant> > DataUtils::filterNumeric(const QList<QPair<QVariant,QVariant> >& data) {
    QList<QPair<QVariant,QVariant> > result;
    for(int i=0;i<data.size();i++) {
        const QVariant& v1 = data[i].first;
        const QVariant& v2 = data[i].second;
        if (v1.isNull() || v2.isNull() || !qIsNumericType(v1.type()) || !qIsNumericType(v2.type())) {
            continue;
        }
        result.append(data[i]);
    }
    return result;
}

QVariantList DataUtils::filterNumeric(const QVariantList& data) {
    QVariantList result;
    foreach (const QVariant& v, data) {
        if (!v.isNull() && qIsNumericType(v.type())) {
            result << v;
        }
    }
    return result;
}


QVariantList DataUtils::columnData(const QAbstractItemModel* model,int column) {
    QVariantList result;
    if (column < 0) {
        for(int row = 0; row < model->rowCount(); row++) {
            result << row;
        }
    } else {
        for(int row = 0; row < model->rowCount(); row++) {
            result << model->data(model->index(row,column));
        }
    }
    return result;
}

QStringList DataUtils::toLower(const QStringList& vs) {
    QStringList result;
    foreach(const QString& v, vs) {
        result << v.toLower();
    }
    return result;
}


QStringList DataUtils::headerData(const QAbstractItemModel* model, Qt::Orientation orientation) {
    QStringList result;
    int count = orientation == Qt::Horizontal ? model->columnCount() : model->rowCount();
    for(int section=0;section<count;section++) {
        result.append(model->headerData(section,orientation).toString());
    }
    return result;
}


QStringList DataUtils::filterEmpty(const QStringList& items) {
    QStringList res;
    foreach(const QString& item, items) {
        if (!item.isEmpty()) {
            res << item;
        }
    }
    return res;
}

QList<double> DataUtils::toDouble(const QVariantList& vs) {
    QList<double> res;
    QVariant v;
    foreach(v,vs) {
        res.append(v.toDouble());
    }
    return res;
}

