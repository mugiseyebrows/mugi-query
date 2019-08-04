#ifndef TIMEZONE_H
#define TIMEZONE_H
#include <QByteArray>
#include <QString>

class TimeZone {
public:
    TimeZone();
    TimeZone(const QString& code, const QString& name, const QByteArray& ianaId, const QString& utc,
             int offset);
    QString code() const;
    void setCode(const QString& code);
    QString name() const;
    void setName(const QString& name);
    QByteArray ianaId() const;
    void setIanaId(const QByteArray& ianaId);
    QString utc() const;
    void setUtc(const QString& utc);
    int offset() const;
    void setOffset(int offset);
    bool isValid() const;

protected:
    QString mCode;
    QString mName;
    QByteArray mIanaId;
    QString mUtc;
    int mOffset;
};
#endif // TIMEZONE_H
