#ifndef SELECTCOLUMNSWIDGET_H
#define SELECTCOLUMNSWIDGET_H

#include <QWidget>

namespace Ui {
class SelectColumnsWidget;
}

class SelectColumnsListWidget;

#include "dataformat.h"
#include <QModelIndex>

class SelectColumnsWidget : public QWidget
{
    Q_OBJECT

public:
    enum LabelsMode {
        LabelsModeShort,
        LabelsModeLong
    };

    explicit SelectColumnsWidget(QWidget *parent = nullptr);
    ~SelectColumnsWidget();

    void setTable(const QString& table);
    void setFormat(DataFormat::Format format);

    SelectColumnsListWidget * data() const;
    SelectColumnsListWidget * keys() const;

    void setFields(const QStringList &fields, const QList<QMetaType::Type> &types = {});

    void checked(QStringList &fields, QList<QMetaType::Type> &types);

    void dataChecked(QStringList &fields, QList<QMetaType::Type> &types);
    void keysChecked(QStringList &fields, QList<QMetaType::Type> &types);

    void setLabelsMode(LabelsMode mode);

    bool hasAnyChecked() const;

    void setLabelText(const QString &dataLabel, const QString &keysLabel);
    QList<bool> keysCheckList() const;
    QList<bool> dataCheckList() const;
signals:
    void dataChanged(int,QModelIndex,QModelIndex);

protected:
    Ui::SelectColumnsWidget *ui;
    DataFormat::Format mFormat;
    QString mTable;
    LabelsMode mLabelsMode;
};

#endif // SELECTCOLUMNSWIDGET_H
