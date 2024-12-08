#ifndef COLUMNPOSITION_H
#define COLUMNPOSITION_H

class SqlEscaper;
#include <QString>

class ColumnPosition {
public:
    enum Position {
        Unknown,
        After,
        First,
    };
    ColumnPosition(Position position = Unknown, const QString& name = QString()) :
        mPosition(position), mName(name)
    {

    }

    QString toString(const SqlEscaper& es) const;

protected:
    Position mPosition;
    QString mName;

    /*static ColumnPosition after(const QString& name) {

    }
    static ColumnPosition first() {

    }*/
};


#endif // COLUMNPOSITION_H
