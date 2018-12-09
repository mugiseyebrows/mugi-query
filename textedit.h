#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QPlainTextEdit>

class QCompleter;
class Highlighter;
class Tokens;

class TextEdit : public QTextEdit
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = 0);
    ~TextEdit();

    void setTokens(const Tokens &tokens);
signals:
    void submit();

protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);

    void setCompleter(QCompleter *completer);
    QCompleter *completer() const;

    void setHighlighter(Highlighter* highlighter);
    Highlighter* highlighter() const;


private slots:
    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;

private:
    QCompleter *mCompleter;
    Highlighter* mHighlighter;

};

#endif // TEXTEDIT_H

