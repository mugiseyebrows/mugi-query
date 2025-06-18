#include "copyformat2.h"

CopyFormat2::Format CopyFormat2::format() const {
    return mFormat;
}

CopyFormat2::Sep CopyFormat2::sep() const {
    return mSep;
}

CopyFormat2::Lang CopyFormat2::lang() const {
    return mLang;
}

CopyFormat2::Axis CopyFormat2::axis() const {
    return mAxis;
}

QStringList CopyFormat2::key() const {
    return mKey;
}

QStringList CopyFormat2::formats() {
    return {
        "tsv",
        "sql",
        "json",
        "list",
        "dict",
        "df"
    };
}

QStringList CopyFormat2::seps() {
    return {".",
        ","};
}

QStringList CopyFormat2::langs() {
    return {
        "c++",
        "py",
        "js"
    };
}

QStringList CopyFormat2::axes() {
    return {
        "rows",
        "cols",
    };
}
