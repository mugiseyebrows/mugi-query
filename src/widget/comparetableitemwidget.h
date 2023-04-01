#ifndef COMPARETABLEITEMWIDGET_H
#define COMPARETABLEITEMWIDGET_H

#include <QWidget>

namespace Ui {
class CompareTableItemWidget;
}

class CompareTableItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CompareTableItemWidget(QWidget *parent = nullptr);
    ~CompareTableItemWidget();

    void init(const QStringList& connectionNames);

    QString database() const;

    QString table() const;

private slots:
    void on_database_currentIndexChanged(int index);

private:
    Ui::CompareTableItemWidget *ui;
};

#endif // COMPARETABLEITEMWIDGET_H
