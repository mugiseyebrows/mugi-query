#ifndef ZIPUNZIP_H
#define ZIPUNZIP_H

#include <QList>
#include <QPair>

template<typename T1, typename T2>
QList<QPair<T1,T2> > zipToPairList(const QList<T1>& vs1, const QList<T2>& vs2) {
    QList<QPair<T1,T2> > res;
    for(int i=0;i<vs1.size();i++) {
        res.append(QPair<T1,T2>(vs1[i],vs2[i]));
    }
    return res;
}

template<typename T1, typename T2>
QList<T1> unzipFirsts(const QList<QPair<T1,T2> >& vs) {
    QList<T1> res;
    for(int i=0;i<vs.size();i++) {
        res.append(vs[i].first);
    }
    return res;
}

template<typename T1, typename T2>
QList<T2> unzipSeconds(const QList<QPair<T1,T2> >& vs) {
    QList<T2> res;
    for(int i=0;i<vs.size();i++) {
        res.append(vs[i].second);
    }
    return res;
}



#endif // ZIPUNZIP_H
