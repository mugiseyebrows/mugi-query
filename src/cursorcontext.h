#ifndef CURSORCONTEXT_H
#define CURSORCONTEXT_H

class QTextCursor;
#include <QSet>
#include <QString>

class CursorContext
{
public:
    enum Context {
        Undefined,
        Select,
        From,
        Join,
        Where,
        Having,
        On,
        Update,
        Set,
        Table,
        To,
        Column,
        Call,
        Start,
        Insert,
        Into,
        Rename,
        Create,
    };

    explicit CursorContext(Context context = Undefined, const QString& table = {}) : context(context), table(table) {

    }

    bool isFieldContext() const;

    bool isTableContext() const;

    static QSet<Context> fieldContexts();
    static QSet<Context> tableContexts();


    operator ==(const CursorContext& lhs);
    operator !=(const CursorContext& lhs);

    Context context;
    QString table;

    static CursorContext getContext(const QTextCursor &cur);
};

QDebug operator << (QDebug debug, const CursorContext& ctx);

#endif // CURSORCONTEXT_H
