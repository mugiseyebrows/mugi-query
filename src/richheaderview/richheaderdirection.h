#ifndef RICHHEADERDIRECTION_H
#define RICHHEADERDIRECTION_H

#include <QDebug>

class RichHeaderDirection {
public:
    enum DirectionType {
        DirectionUp,
        DirectionRight,
        DirectionDown,
        DirectionLeft
    };

    static bool horizontal(RichHeaderDirection::DirectionType direction);

    static DirectionType orthogonal(RichHeaderDirection::DirectionType direction);

    static bool vertical(RichHeaderDirection::DirectionType direction);

    static bool ascending(RichHeaderDirection::DirectionType direction);

    static RichHeaderDirection::DirectionType horizontal(RichHeaderDirection::DirectionType dir1, RichHeaderDirection::DirectionType dir2);
    static RichHeaderDirection::DirectionType vertical(RichHeaderDirection::DirectionType dir1, RichHeaderDirection::DirectionType dir2);

    static DirectionType inverted(RichHeaderDirection::DirectionType direction);
    static DirectionType maybeInverted(RichHeaderDirection::DirectionType direction, bool inverted);

};

QDebug operator<<(QDebug debug, RichHeaderDirection::DirectionType);

#endif // RICHHEADERDIRECTION_H
