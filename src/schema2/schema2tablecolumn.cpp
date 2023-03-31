#include "schema2tablecolumn.h"

Schema2TableColumn::Schema2TableColumn() : notNull(false) {}

Schema2TableColumn::Schema2TableColumn(const QString &name, const QString &type, bool notNull) : name(name), type(type), notNull(notNull) {

}
