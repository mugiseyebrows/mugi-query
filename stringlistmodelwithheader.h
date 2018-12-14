#ifndef STRINGLISTMODELWITHHEADER_H
#define STRINGLISTMODELWITHHEADER_H

#include <QStringListModel>

class StringListModelWithHeader : public QStringListModel
{
    Q_OBJECT
public:
    StringListModelWithHeader(const QStringList &strings = QStringList(), QObject *parent = nullptr);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

protected:
    QMap<int,QVariant> mHorizontalHeader;
    QMap<int,QVariant> mVerticalHeader;
};

#endif // STRINGLISTMODELWITHHEADER_H
