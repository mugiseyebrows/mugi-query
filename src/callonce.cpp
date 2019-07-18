#include "callonce.h"

#include <QTimer>

CallOnce::CallOnce(const QString name, QObject *parent) : QObject(parent), mName(name), mId(0)
{

}

void CallOnce::onPost()
{
    mId++;
    int id = mId;
    QTimer::singleShot(0,[=](){
        if (id == mId) {
            qDebug() << mName << "id == mId" << id << mId;
            emit call();
        } else {
            //qDebug() << mName << "id != mId" << id << mId;
        }
    });
}

