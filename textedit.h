#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include "tokens.h"
#include <QMap>

class QCompleter;
class Highlighter;


class TextEdit : public QTextEdit
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = 0);
    ~TextEdit();

    void setTokens(const Tokens &tokens);
    void updateCompleter();
signals:
    void submit();

protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);

    void setCompleter(QCompleter *completer);
    QCompleter *completer() const;

    void setHighlighter(Highlighter* highlighter);
    Highlighter* highlighter() const;


protected slots:
    void onTextChanged();
    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;

private:
    QCompleter *mCompleter;
    Highlighter* mHighlighter;
    Tokens mTokens;
    QMap<QString,QString> mAliases;
};

#endif // TEXTEDIT_H

