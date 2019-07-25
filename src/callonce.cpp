#include "callonce.h"

#include <QTimer>

CallOnce::CallOnce(const QString name, int timeout, QObject *parent) : QObject(parent), mName(name), mId(0), mTimeout(timeout)
{

}

void CallOnce::onPost()
{
    mId++;
    int id = mId;
    QTimer::singleShot(mTimeout,[=](){
        if (id == mId) {
            //qDebug() << mName << "id == mId" << id << mId;
            emit call();
        } else {
            //qDebug() << mName << "id != mId" << id << mId;
        }
    });
}

