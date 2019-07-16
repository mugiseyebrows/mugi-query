#ifndef DATAIMPORTWIDGETS_H
#define DATAIMPORTWIDGETS_H

#include <QWidget>

class DataImportWidget;
class Tokens;

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

public slots:
    void update(const QString &connectionName, const Tokens &tokens, bool focus);
signals:
    void appendQuery(QString,QString);
protected:
    void closeTab(const QString &connectionName);
private:
    Ui::DataImportWidgets *ui;
};

#endif // DATAIMPORTWIDGETS_H
