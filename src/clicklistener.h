#ifndef CLICKLISTENER_H
#define CLICKLISTENER_H

#include <QObject>

class ClickListener : public QObject
{
    Q_OBJECT
public:
    explicit ClickListener(QObject *parent = nullptr);

signals:
    void tableClicked(QString);
};

#endif // CLICKLISTENER_H
