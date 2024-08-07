#ifndef SHOWHIDEFILTER_H
#define SHOWHIDEFILTER_H

#include <QObject>

class ShowHideFilter : public QObject
{
    Q_OBJECT
public:
    explicit ShowHideFilter(QObject *parent = nullptr);

signals:
    void shown();
    void hidden();
    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // SHOWHIDEFILTER_H
