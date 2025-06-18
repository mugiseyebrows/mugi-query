#ifndef COPYFORMAT2_H
#define COPYFORMAT2_H

#include <QStringList>

class CopyFormat2 {
public:
    enum Format {
        Tsv,
        Sql,
        Json,
        List,
        Dict,
        Df
    };
    enum Sep {
        Dot,
        Comma
    };
    enum Lang {
        Cpp,
        Py,
        Js
    };
    enum Axis {
        Rows,
        Cols
    };

    CopyFormat2(Format format = Tsv, Sep sep = Dot, Lang lang = Py, Axis axis = Rows, const QStringList key = {}) :
        mFormat(format), mSep(sep), mLang(lang), mAxis(axis), mKey(key) {

    }

    Format format() const;

    Sep sep() const;

    Lang lang() const;

    Axis axis() const;

    QStringList key() const;

    static bool sepRelevant(Format format) {
        return format == Tsv;
    }

    static bool langRelevant(Format format) {
        return format == List || format == Dict;
    }

    static bool axisRelevant(Format format) {
        return format == Tsv || format == List;
    }

    static bool keyRelevant(Format format) {
        return format == Dict;
    }

    static QStringList formats();

    static QStringList seps();

    static QStringList langs();

    static QStringList axes();

protected:
    Format mFormat = Tsv;
    Sep mSep = Dot;
    Lang mLang = Cpp;
    Axis mAxis = Rows;
    QStringList mKey;
};

#endif // COPYFORMAT2_H
