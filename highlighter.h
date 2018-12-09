#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

class Highlighters;
class Tokens;

class Highlighter : public QSyntaxHighlighter
{
public:
    Highlighter(const Tokens& tokens, QTextDocument *parent);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat tableFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat specialCharsFormat;
    QTextCharFormat typesFormat;

};

#endif // HIGHLIGHTER_H
