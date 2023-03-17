#ifndef XJOINWIDGET_H
#define XJOINWIDGET_H

#include <QWidget>

namespace Ui {
class XJoinWidget;
}

class XJoinWidget : public QWidget
{
    Q_OBJECT

public:
    explicit XJoinWidget(QWidget *parent = nullptr);
    ~XJoinWidget();

    void init(const QStringList& connectionNames);

protected slots:

    void updateResults();

protected:

private:
    Ui::XJoinWidget *ui;
};

#endif // XJOINWIDGET_H
