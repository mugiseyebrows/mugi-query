#ifndef RELATETOOLWIDGET_H
#define RELATETOOLWIDGET_H

#include <QWidget>

namespace Ui {
class RelateToolWidget;
}

class RelateToolWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RelateToolWidget(QWidget *parent = nullptr);
    ~RelateToolWidget();

signals:
    void selected(QString, QString);

public slots:
    void clear();
    void push(QString);

private slots:
    void on_clearParent_clicked();

    void on_clearChild_clicked();

private:
    Ui::RelateToolWidget *ui;
};

#endif // RELATETOOLWIDGET_H
