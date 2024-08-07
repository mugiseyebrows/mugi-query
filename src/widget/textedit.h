#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include "tokens.h"
#include <QMap>

class Completer;
class Highlighter;

class TextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = 0);
    ~TextEdit();

    void setTokens(const Tokens &tokens);
    Tokens tokens() const;
    void updateCompleter();

    void setText(const QString& text);

signals:
    void submit();

protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);

#if 0
    void setCompleter(QCompleter *completer);
    QCompleter *completer() const;
#endif

    void setHighlighter(Highlighter* highlighter);
    Highlighter* highlighter() const;


    bool isCompeleterVisible();
    bool completerForwardedToIgnore(QKeyEvent *e);
    void updateCompleter(QKeyEvent *e);
    void showCompleter(bool testPrefixLength);
    void hideCompleter();
    bool keyPressEventCompleter(QKeyEvent *e);
    void paintEvent(QPaintEvent *e);
    QPainterPath createSelectionPath(const QTextCursor &begin, const QTextCursor &end, const QRect &clip);
    bool keyPressEventEdits(QKeyEvent *event);
    void clearEdits();
    void fixEdits();
    bool tryEmmet();
    bool cursorAtEndOfWord() const;
    void moveTextCursorToEdit(int index);
    int prevEditIndex() const;
    int nextEditIndex() const;
    int editIndex() const;
protected slots:
    void onTextChanged();
    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;

private:
    Completer *mCompleter;
    Highlighter* mHighlighter;
    Tokens mTokens;
    QMap<QString,QString> mAliases;
    QList<QPair<QTextCursor, QTextCursor>> m_edits;
};

#endif // TEXTEDIT_H

