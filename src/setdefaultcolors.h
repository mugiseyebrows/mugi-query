#ifndef SETDEFAULTCOLORS_H
#define SETDEFAULTCOLORS_H

#include <QList>
class ModelAppender;
class QAbstractItemModel;

void setDefaultColors(ModelAppender* appender, QAbstractItemModel* model,
                                const QList<int>& nonEmpty, const QList<int>& nthColor,
                                const QList<int>& noneColor);

#endif // SETDEFAULTCOLORS_H
