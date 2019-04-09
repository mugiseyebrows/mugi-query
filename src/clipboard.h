#ifndef CLIPBOARD_H
#define CLIPBOARD_H

class QTextStream;
class QAbstractItemModel;
class QItemSelection;
class QString;
class QLocale;
class QItemSelectionRange;
class QModelIndex;

#include "dataformat.h"

class Clipboard
{
public:

    static void copySelected(QAbstractItemModel *model, const QItemSelection& selection,
                             DataFormat::Format format, const QString& separator,
                             const QLocale &locale, QString &error);

    static void copySelectedAsList(QAbstractItemModel *model,
                                   const QItemSelection& selection,
                                   const QLocale &locale, QString &error);

    static void streamRange(QTextStream &stream, const QItemSelectionRange &rng,
                            DataFormat::Format format, const QString &separator,
                            DataFormat::ActionType action, const QLocale &locale, QString &error);

    static void pasteTsv(QAbstractItemModel *model, const QModelIndex& index);

};

#endif // CLIPBOARD_H
