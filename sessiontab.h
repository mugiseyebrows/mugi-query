#ifndef SESSIONTAB_H
#define SESSIONTAB_H

#include <QWidget>

namespace Ui {
class SessionTab;
}

class SessionTab : public QWidget
{
    Q_OBJECT
    
public:
    explicit SessionTab(const QString& connectionName, const QString name, QWidget *parent = 0);
    ~SessionTab();

    QString connectionName() const;
    QString name() const;

public slots:
    void on_execute_clicked();

signals:
    void query(QString);
    
protected:

    QString mConnectionName;
    QString mName;

    Ui::SessionTab *ui;
};

#endif // SESSIONTAB_H
