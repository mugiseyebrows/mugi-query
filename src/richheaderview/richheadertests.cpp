#include "richheadertests.h"

#include <QDebug>
#include "richheaderdataimpl.h"

RichHeaderTests::RichHeaderTests()
{

}

bool RichHeaderTests::run()
{
    return testRangesOverlap();
}

bool RichHeaderTests::testRangesOverlap()
{
    bool passed = true;

    for (int i = 0; i < 1000; i++) {

        int a1 = rand() % 20;
        int a2 = a1 + (rand() % 20);
        int b1 = rand() % 20;
        int b2 = b1 + (rand() % 20);

        bool actual = RichHeaderDataImplBase::rangesOverlap(a1, a2, b1, b2);
        QSet<int> a;
        QSet<int> b;

        for (int i = a1; i <= a2; i++) {
            a << i;
        }
        for (int i = b1; i <= b2; i++) {
            b << i;
        }

        bool expected = a.intersect(b).size() > 0;

        if (actual != expected) {
            qDebug() << "failed" << a1 << a2 << b1 << b2 << "expected" << expected << "got"
                     << actual;
            passed = false;
        } else {
            // qDebug() << a1 << a2 << b1 << b2 << actual;
        }
    }

    qDebug() << "testRangesOverlap()" << (passed ? "passed" : "failed");

    return passed;
}
