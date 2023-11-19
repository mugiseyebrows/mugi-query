#include "schema2tablecolumn.h"

Schema2TableColumn::Schema2TableColumn() : notNull(false), autoincrement(false) {}

Schema2TableColumn::Schema2TableColumn(const QString &name, const QString &type, bool notNull, const QString &default_, bool autoincrement) :
    name(name), type(type), notNull(notNull), default_(default_), autoincrement(autoincrement)  {

}
