#ifndef REPORTERROR_H
#define REPORTERROR_H

#include <QString>
class QWidget;

void report_error(const QString& error, const QString &file, int line, QWidget* parent = 0);

#endif // REPORTERROR_H
