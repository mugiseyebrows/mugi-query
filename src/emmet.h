#ifndef EMMET_H
#define EMMET_H

#include <QString>
#include <QTextCursor>

QString parseEmmet(const QString& text);

QTextCursor selectEmmetExpression(const QTextCursor& cursor);

#endif // EMMET_H
