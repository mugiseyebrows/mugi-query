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
    explicit SessionTab(const QString& database, const QString name, QWidget *parent = 0);
    ~SessionTab();

    QString database() const;
    QString name() const;

public slots:
    void on_execute_clicked();

signals:
    void query(QString);
    
protected:

    QString mDatabase;
    QString mName;

    Ui::SessionTab *ui;
};

#endif // SESSIONTAB_H
