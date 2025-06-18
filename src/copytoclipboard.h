#ifndef COPYTOCLIPBOARD_H
#define COPYTOCLIPBOARD_H

class QAbstractItemModel;
class QItemSelection;
class CopyFormat2;
class QSqlDriver;

void copyToClipboard(QAbstractItemModel* model, const QItemSelection& selection,
                     const CopyFormat2& fmt, QSqlDriver *driver);

#endif // COPYTOCLIPBOARD_H
