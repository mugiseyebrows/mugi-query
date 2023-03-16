#include "setcompleter.h"


#include <QCompleter>
#include <QStringListModel>
#include <QLineEdit>

void setCompleter(QLineEdit* lineEdit, const QStringList& completions) {
    QCompleter* completer = new QCompleter(lineEdit);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    QStringListModel* stringListModel = new QStringListModel(completions);
    completer->setModel(stringListModel);
    lineEdit->setCompleter(completer);
}
