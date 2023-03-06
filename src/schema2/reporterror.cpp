#include "reporterror.h"

#include <QMessageBox>
#include <QDebug>

void report_error(const QString &error, const QString &file, int line, QWidget *parent)
{

#if 1
    qDebug() << error << line << file;
#else
    QMessageBox::critical(parent, "Error", QString("%1 @ %2:%3").arg(error).arg(file).arg(line));
#endif

}
