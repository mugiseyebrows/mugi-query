#ifndef TABLEEDITWIDGET_H
#define TABLEEDITWIDGET_H

#include <QWidget>
#include "sdata.h"
#include <QSqlDatabase>

class QLineEdit;
class QSqlTableModel;

namespace Ui {
class TableEditWidget;
}

class TableEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TableEditWidget(QWidget *parent = nullptr);
    ~TableEditWidget();

    void init(QSqlDatabase db, const SName& name);



protected:
    QList<QLineEdit*> mEdits;
    QSqlTableModel* mModel;


    Ui::TableEditWidget *ui;
protected slots:
    void onFilterChanged(const QString &);
};

#endif // TABLEEDITWIDGET_H
