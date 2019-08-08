#ifndef TESTDATETIME2SAMPLE_H
#define TESTDATETIME2SAMPLE_H
#include <QString>

class TestDateTime2Sample {
public:
    TestDateTime2Sample();
    TestDateTime2Sample(const QString& string, bool hasSeconds = false,
                        bool hasMilliseconds = false);
    QString string() const;
    void setString(const QString& string);
    bool hasSeconds() const;
    void setHasSeconds(bool hasSeconds);
    bool hasMilliseconds() const;
    void setHasMilliseconds(bool hasMilliseconds);

protected:
    QString mString;
    bool mHasSeconds;
    bool mHasMilliseconds;
};
#endif // TESTDATETIME2SAMPLE_H
