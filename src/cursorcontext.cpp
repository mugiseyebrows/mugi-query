#include "cursorcontext.h"

#include <QTextCursor>
#include <QRegularExpression>
#include <QTextCursor>


CursorContext CursorContext::getContext(const QTextCursor& cur) {
    QRegularExpression rxWord("([a-z]+)", QRegularExpression::CaseInsensitiveOption);

    static QHash<QString, Context> contexts = {
                                               {"from",From},
                                               {"select",Select},
                                               {"join",Join},
                                               {"where",Where},
                                               {"on",On},
                                               {"update",Update},
                                               {"set",Set},
                                               {"table", Table},
                                               {"to", To},
                                               {"column",Column},
                                               {"call", Call},
                                               {"having", Having},
                                               };

    auto curCopy = cur;
    while (true) {
        if (!curCopy.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor)) {
            return CursorContext{Start};
        }
        if (curCopy.selectedText().contains(";")) {
            //qDebug() << "went too far" << curCopy.selectedText();
            return CursorContext{Start};
        }

        auto m = rxWord.match(curCopy.selectedText());
        if (m.hasMatch()) {
            QString word = m.captured(1).toLower();
            if (contexts.contains(word)) {
                //qDebug() << "contexts.contains(word)" << word;

                if (word == "on") {
                    // todo: movePosition QTextCursor::PreviousWord treats 0x2029 (paragraph separator as a word), do better
                    curCopy.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
                    QRegularExpression tableRx("([^\\s]+)");
                    auto table = tableRx.match(curCopy.selectedText());
                    return CursorContext{On, table.captured(1)};
                }

                return CursorContext{contexts[word]};
            } else {
                qDebug() << "!contexts.contains(word)" << word;
            }
        } else {
            qDebug() << "!m.hasMatch()" << curCopy.selectedText();
        }
        qDebug() << curCopy.selectedText();
    }
    return CursorContext{Undefined};
}

QSet<CursorContext::Context> CursorContext::fieldContexts()
{
    return {Select, On, Where, Having, Set, Column};
}

QSet<CursorContext::Context> CursorContext::tableContexts()
{
    return {From, Join, Update, Table, To};
}

bool CursorContext::isFieldContext() const
{
    return fieldContexts().contains(context);
}

bool CursorContext::isTableContext() const
{
    return tableContexts().contains(context);
}

CursorContext::operator ==(const CursorContext &lhs) {
    return this->context == lhs.context;
}

CursorContext::operator !=(const CursorContext &lhs) {
    return !(*this == lhs);
}

QDebug operator <<(QDebug debug, const CursorContext &ctx) {
    static QStringList names = {"Undefined", "Select", "From", "Join", "Where", "Having", "On", "Update", "Set", "Table", "To", "Column", "Call", "Start"};
    debug << names.value(ctx.context) << ctx.table;
    return debug;
}
