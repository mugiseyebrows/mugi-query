#ifndef SELECTCOLUMNSLISTWIDGET_H
#define SELECTCOLUMNSLISTWIDGET_H

#include <QWidget>
#include <QModelIndex>

class DataImportColumnModel;

namespace Ui {
class SelectColumnsListWidget;
}

class SelectColumnsListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectColumnsListWidget(QWidget *parent = nullptr);
    ~SelectColumnsListWidget();

    void setFields(const QStringList &fields, const QList<QVariant::Type> &types);
    void setLabelText(const QString& text);

    DataImportColumnModel* model() const;

    bool hasAnyChecked() const;
    void setAllUnchecked();
    void setAllChecked();

signals:
    void dataChanged(QModelIndex,QModelIndex);
protected slots:
    void on_all_clicked();

    void on_none_clicked();

protected:
    Ui::SelectColumnsListWidget *ui;

};

#endif // SELECTCOLUMNSLISTWIDGET_H
