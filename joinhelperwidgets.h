#ifndef JOINHELPERWIDGETS_H
#define JOINHELPERWIDGETS_H

#include <QWidget>
#include "tokens.h"
class JoinHelperWidget;

namespace Ui {
class JoinHelperWidgets;
}

class JoinHelperWidgets : public QWidget
{
    Q_OBJECT

public:
    explicit JoinHelperWidgets(QWidget *parent = 0);
    ~JoinHelperWidgets();

    void update(const QString& connectionName, const Tokens &tokens, bool focus);

    void closeTab(const QString& connectionName);

    void save();

protected:

    JoinHelperWidget *tab(int index);

    int tabIndex(const QString &connectionName);

signals:
    void appendQuery(QString,QString);

private:
    Ui::JoinHelperWidgets *ui;
};

#endif // JOINHELPERWIDGETS_H
