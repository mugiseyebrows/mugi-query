#ifndef TESTS_H
#define TESTS_H

namespace Tests
{
    void run();

    bool testSplit();
    bool testJoinSplit();
    bool testAliases();
    bool testFlatQueries();
    bool testClosingBracket();
    bool testParseCreateTable();
    bool textParseCreateTableCreateDefinition();
    //void testDateRegularExpressions();
    //void testDateTimeRegularExpressions();
    bool testTryConvert1();
    bool testTryConvert2();
    bool testApParse();
    bool testTimeZones();
    bool testDateTimeParse();
    bool testTableNamesFromSelectQuery();
}

#endif // TESTS_H
