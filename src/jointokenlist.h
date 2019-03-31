#ifndef JOINTOKENLIST_H
#define JOINTOKENLIST_H
#include <QPair>
#include <QString>
#include <QList>
#include "jointoken.h"

namespace JoinTokenList {
QList<QPair<JoinToken::JoinToken,QString> > jtl();
QList<QPair<JoinToken::JoinToken,QString> > jtl(JoinToken::JoinToken a0, const QString& b0);
QList<QPair<JoinToken::JoinToken,QString> > jtl(JoinToken::JoinToken a0, const QString& b0, JoinToken::JoinToken a1, const QString& b1);
QList<QPair<JoinToken::JoinToken,QString> > jtl(JoinToken::JoinToken a0, const QString& b0, JoinToken::JoinToken a1, const QString& b1, JoinToken::JoinToken a2, const QString& b2);
QList<QPair<JoinToken::JoinToken,QString> > jtl(JoinToken::JoinToken a0, const QString& b0, JoinToken::JoinToken a1, const QString& b1, JoinToken::JoinToken a2, const QString& b2, JoinToken::JoinToken a3, const QString& b3);
QList<QPair<JoinToken::JoinToken,QString> > jtl(JoinToken::JoinToken a0, const QString& b0, JoinToken::JoinToken a1, const QString& b1, JoinToken::JoinToken a2, const QString& b2, JoinToken::JoinToken a3, const QString& b3, JoinToken::JoinToken a4, const QString& b4);
QList<QPair<JoinToken::JoinToken,QString> > jtl(JoinToken::JoinToken a0, const QString& b0, JoinToken::JoinToken a1, const QString& b1, JoinToken::JoinToken a2, const QString& b2, JoinToken::JoinToken a3, const QString& b3, JoinToken::JoinToken a4, const QString& b4, JoinToken::JoinToken a5, const QString& b5);
QList<QPair<JoinToken::JoinToken,QString> > jtl(JoinToken::JoinToken a0, const QString& b0, JoinToken::JoinToken a1, const QString& b1, JoinToken::JoinToken a2, const QString& b2, JoinToken::JoinToken a3, const QString& b3, JoinToken::JoinToken a4, const QString& b4, JoinToken::JoinToken a5, const QString& b5, JoinToken::JoinToken a6, const QString& b6);
QList<QPair<JoinToken::JoinToken,QString> > jtl(JoinToken::JoinToken a0, const QString& b0, JoinToken::JoinToken a1, const QString& b1, JoinToken::JoinToken a2, const QString& b2, JoinToken::JoinToken a3, const QString& b3, JoinToken::JoinToken a4, const QString& b4, JoinToken::JoinToken a5, const QString& b5, JoinToken::JoinToken a6, const QString& b6, JoinToken::JoinToken a7, const QString& b7);
QList<QPair<JoinToken::JoinToken,QString> > jtl(JoinToken::JoinToken a0, const QString& b0, JoinToken::JoinToken a1, const QString& b1, JoinToken::JoinToken a2, const QString& b2, JoinToken::JoinToken a3, const QString& b3, JoinToken::JoinToken a4, const QString& b4, JoinToken::JoinToken a5, const QString& b5, JoinToken::JoinToken a6, const QString& b6, JoinToken::JoinToken a7, const QString& b7, JoinToken::JoinToken a8, const QString& b8);

}

#endif // JOINTOKENLIST_H
