#ifndef CORNERBUTTON_H
#define CORNERBUTTON_H

#include <QObject>
class QPushButton;

class CornerButton : public QObject
{
    Q_OBJECT
public:
    explicit CornerButton(const QString& text, QObject *parent = nullptr);

    QPushButton* button() const;

    void setWidget(QWidget* widget);

protected:
    QPushButton* mButton;
    QWidget* mParent;

signals:


    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // CORNERBUTTON_H
