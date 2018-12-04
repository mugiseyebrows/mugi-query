#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QPlainTextEdit>

class QCompleter;

class PlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    PlainTextEdit(QWidget *parent = 0);
    ~PlainTextEdit();

    void setCompleter(QCompleter *mCompleter);
    QCompleter *completer() const;

signals:
    void submit();

protected:
    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);


private slots:
    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;

private:
    QCompleter *mCompleter;
};

#endif // TEXTEDIT_H

