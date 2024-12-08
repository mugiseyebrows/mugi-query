#include "columnposition.h"

#include "sqlescaper.h"

QString ColumnPosition::toString(const SqlEscaper &es) const {
    switch (mPosition) {
    case Unknown: return {};
    case After: return QString("AFTER %1").arg(es.field(mName));
    case First: return "FIRST";
    }
}
