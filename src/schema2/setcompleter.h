#ifndef SETCOMPLETER_H
#define SETCOMPLETER_H

class QLineEdit;
class QStringList;

void setCompleter(QLineEdit* lineEdit, const QStringList& completions);

#endif // SETCOMPLETER_H
