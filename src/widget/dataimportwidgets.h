#ifndef DATAIMPORTWIDGETS_H
#define DATAIMPORTWIDGETS_H

#include <QWidget>

class DataImportWidget;
#include "tokens.h"

namespace Ui {
class DataImportWidgets;
}

class DataImportWidgets : public QWidget
{
    Q_OBJECT

public:
    explicit DataImportWidgets(QWidget *parent = nullptr);
    ~DataImportWidgets();
    int tabIndex(const QString &connectionName);
    DataImportWidget *tab(int index);

    void update(const QString &connectionName, const Tokens &tokens);
    void create(const QString &connectionName, const Tokens &tokens);
protected:
    QMap<QString,Tokens> mTokens;


signals:
    void appendQuery(QString,QString);
    void updateTokens(QString,Tokens);
protected:
    void closeTab(const QString &connectionName);
private:
    Ui::DataImportWidgets *ui;
};

#endif // DATAIMPORTWIDGETS_H
