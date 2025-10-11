#ifndef DUMP_H
#define DUMP_H

#include <QStringList>

void dump(const QString& path, const QStringList& lines);

QString nextPath(const QString& pattern);

#endif // DUMP_H
