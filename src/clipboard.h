#ifndef CLIPBOARD_H
#define CLIPBOARD_H

class QTextStream;
class QAbstractItemModel;
class QItemSelection;
class QString;
class QLocale;
class QItemSelectionRange;
class QModelIndex;
class QSqlQueryModel;

#include "dataformat.h"

class Clipboard
{
public:

    static void copySelected(QAbstractItemModel *model, const QItemSelection& selection,
                             DataFormat::Format format, const QString& separator, bool header,
                             const QLocale &locale, QString &error);

    static void copySelectedAsList(QSqlQueryModel *model,
                                   const QItemSelection& selection);

    static void copySelectedNames(QSqlQueryModel *model,
                                  const QItemSelection& selection);

    static void streamRange(QTextStream &stream, const QItemSelectionRange &rng,
                            DataFormat::Format format, const QString &separator,
                            DataFormat::ActionType action, const QLocale &locale, QString &error);

    static QModelIndex pasteTsv(QAbstractItemModel *model, const QModelIndex& index,
                                bool appendRows = false, bool appendColumns = false);

    static void streamHeader(QTextStream &stream, const QItemSelectionRange &rng, const QString &separator, const QString &end = "\n");
    static void streamHeader(QTextStream &stream, QSqlQueryModel *model, const QString &separator, const QString &end = "\n");
    static QString selectedToString(QAbstractItemModel *model, const QItemSelection &selection, DataFormat::Format format, const QString &separator, bool header, const QLocale &locale, QString &error);
    static void copySelectedAsCondition(QSqlQueryModel *model, const QItemSelection &selection);

};

#endif // CLIPBOARD_H
