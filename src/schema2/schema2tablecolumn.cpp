#include "schema2tablecolumn.h"

Schema2TableColumn::Schema2TableColumn() : notNull(false), autoincrement(false) {}

Schema2TableColumn::Schema2TableColumn(const QString &name, const QString &type, bool notNull, bool autoincrement) :
    name(name), type(type), notNull(notNull), autoincrement(autoincrement)  {

}
