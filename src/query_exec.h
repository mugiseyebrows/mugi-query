#ifndef QUERY_EXEC_H
#define QUERY_EXEC_H

#include <QMessageBox>
#include <QSqlError>

#define QUERY_EXEC(q) do { if (!q.exec()) QMessageBox::critical(0,"Error",q.lastError().text()); } while (0)

#endif // QUERY_EXEC_H
