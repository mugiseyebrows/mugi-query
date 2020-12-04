#include "richheaderdirection.h"

QDebug operator<<(QDebug debug, RichHeaderDirection::DirectionType value) {

    const char* vs[] = {"DirectionUp","DirectionRight","DirectionDown","DirectionLeft"};
    debug.nospace() << vs[value];
    return debug.space();
}

bool RichHeaderDirection::horizontal(RichHeaderDirection::DirectionType direction) {
    return direction == RichHeaderDirection::DirectionRight || direction == RichHeaderDirection::DirectionLeft;
}

RichHeaderDirection::DirectionType RichHeaderDirection::orthogonal(RichHeaderDirection::DirectionType direction) {
    return horizontal(direction) ? DirectionDown : DirectionRight;
}

bool RichHeaderDirection::vertical(RichHeaderDirection::DirectionType direction) {
    return direction == RichHeaderDirection::DirectionUp || direction == RichHeaderDirection::DirectionDown;
}

bool RichHeaderDirection::ascending(RichHeaderDirection::DirectionType direction) {
    return direction == RichHeaderDirection::DirectionRight || direction == RichHeaderDirection::DirectionDown;
}


RichHeaderDirection::DirectionType RichHeaderDirection::horizontal(RichHeaderDirection::DirectionType dir1, RichHeaderDirection::DirectionType dir2) {
    return RichHeaderDirection::horizontal(dir1) ? dir1 : dir2;
}

RichHeaderDirection::DirectionType RichHeaderDirection::vertical(RichHeaderDirection::DirectionType dir1, RichHeaderDirection::DirectionType dir2) {
    return RichHeaderDirection::vertical(dir1) ? dir1 : dir2;
}

RichHeaderDirection::DirectionType RichHeaderDirection::inverted(RichHeaderDirection::DirectionType direction)
{
    static QMap<RichHeaderDirection::DirectionType,RichHeaderDirection::DirectionType> m;
    if (m.isEmpty()) {
        m[DirectionUp] = DirectionDown;
        m[DirectionLeft] = DirectionRight;
        m[DirectionDown] = DirectionUp;
        m[DirectionRight] = DirectionLeft;
    }
    return m[direction];
}

RichHeaderDirection::DirectionType RichHeaderDirection::maybeInverted(RichHeaderDirection::DirectionType direction, bool inverted_)
{
    return inverted_ ? inverted(direction) : direction;
}

