#ifndef TESTS_H
#define TESTS_H

namespace Tests
{
    void run();

    void testSplit();
    void testJoinSplit();
    void testAliases();
    void testFlatQueries();
    void testClosingBracket();
    void testParseCreateTable();
    void textParseCreateTableCreateDefinition();
    //void testDateRegularExpressions();
    //void testDateTimeRegularExpressions();
    void testTryConvert1();
    void testTryConvert2();
    bool testApParse();
    bool testTimeZones();
}

#endif // TESTS_H
