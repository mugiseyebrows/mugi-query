#include "qisnumerictype.h"

#include <QMetaType>

bool qIsNumericType(uint tp)
{
    static const qulonglong numericTypeBits =
            Q_UINT64_C(1) << QMetaType::Bool |
                             Q_UINT64_C(1) << QMetaType::Double |
                             Q_UINT64_C(1) << QMetaType::Float |
                             Q_UINT64_C(1) << QMetaType::Char |
                             Q_UINT64_C(1) << QMetaType::SChar |
                             Q_UINT64_C(1) << QMetaType::UChar |
                             Q_UINT64_C(1) << QMetaType::Short |
                             Q_UINT64_C(1) << QMetaType::UShort |
                             Q_UINT64_C(1) << QMetaType::Int |
                             Q_UINT64_C(1) << QMetaType::UInt |
                             Q_UINT64_C(1) << QMetaType::Long |
                             Q_UINT64_C(1) << QMetaType::ULong |
                             Q_UINT64_C(1) << QMetaType::LongLong |
                             Q_UINT64_C(1) << QMetaType::ULongLong;
    return tp < (CHAR_BIT * sizeof numericTypeBits) ? numericTypeBits & (Q_UINT64_C(1) << tp) : false;
}
