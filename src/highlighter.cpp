#include "highlighter.h"

#include "tokens.h"

#include <QDebug>

QStringList spaceSplit(const QStringList& items) {
    QStringList res;
    foreach (const QString& item, items) {
        res << item.split(" ");
    }
    return res;
}

QStringList wordBounded(const QStringList& items) {
    QStringList res;
    foreach (const QString& item, items) {
        res << "\\b" + item + "\\b";
    }
    return res;
}

static QStringList wordBoundedWithBracket(const QStringList& items) {
    QStringList res;
    foreach (const QString& item, items) {
        res << "\\b" + item + "\\s*(?=\\()";
    }
    return res;
}
static QStringList wordBoundedWithoutBracket(const QStringList& items) {
    QStringList res;
    foreach (const QString& item, items) {
        res << "\\b" + item + "\\s*(?![(_a-z])";
    }
    return res;
}
static QStringList wordBoundedWithSize(const QStringList& items) {
    QStringList res;
    foreach (const QString& item, items) {
        res.append("\\b" + item + "\\b\\([0-9,]+\\)");
    }
    return res;
}


Highlighter::Highlighter(const Tokens &tokens, QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    //keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns = wordBounded(spaceSplit(tokens.keywords()));
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern,QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    //tableFormat.setFontWeight(QFont::Bold);
    tableFormat.setForeground(Qt::darkMagenta);
    QStringList tablePatterns = wordBounded(tokens.tablesAndFields(false));
    foreach (const QString &pattern, tablePatterns) {
        rule.pattern = QRegularExpression(pattern,QRegularExpression::CaseInsensitiveOption);
        rule.format = tableFormat;
        highlightingRules.append(rule);
    }

    //functionFormat.setFontWeight(QFont::Bold);
    functionFormat.setForeground(Qt::red);
    QStringList functionPatterns = wordBoundedWithBracket(tokens.functions());
    foreach (const QString &pattern, functionPatterns) {
        rule.pattern = QRegularExpression(pattern,QRegularExpression::CaseInsensitiveOption);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }

    typesFormat.setFontWeight(QFont::Bold);
    QStringList typePatterns = wordBoundedWithoutBracket(tokens.types())
                               + wordBoundedWithSize(tokens.sizedTypes());
    foreach (const QString &pattern, typePatterns) {
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = typesFormat;
        highlightingRules.append(rule);
    }

    specialCharsFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegularExpression(";");
    rule.format = specialCharsFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegularExpression("(?<!/)[*]");
    rule.format = specialCharsFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::gray);
    rule.pattern = QRegularExpression("--[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGray);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("'[^']*'");
    rule.format = quotationFormat;
    highlightingRules.append(rule);




    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");

    //qDebug() << highlightingRules.size() << "rules";

}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                    + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
