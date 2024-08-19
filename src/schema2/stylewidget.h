#ifndef STYLEWIDGET_H
#define STYLEWIDGET_H

#include <QWidget>
class QLineEdit;

namespace Ui {
class StyleWidget;
}

class StyleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StyleWidget(QWidget *parent = nullptr);
    ~StyleWidget();

private slots:
    void on_save_clicked();

protected:
    Ui::StyleWidget *ui;
    QList<QLineEdit*> mEdits;
};

#endif // STYLEWIDGET_H
