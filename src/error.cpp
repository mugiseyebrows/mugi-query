#include "error.h"

#include <QWidget>
#include <QString>
#include <QMessageBox>

void Error::show(QWidget *widget, const QString &error)
{
    if (error.isEmpty()) {
        return;
    }
    QMessageBox::critical(widget,"Error",error);
}
