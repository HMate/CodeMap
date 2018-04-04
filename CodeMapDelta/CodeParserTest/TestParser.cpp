#include <QtTest>

#include "codeparser.h"

class TestParser : public QObject
{
    Q_OBJECT
private slots:
    void testFoo()
    {
        QString code =
                "#define TEST 2\n"
                "int add2(int input){\n"
                "   return input+TEST\n"
                "}";
        QString expected =
                "\nint add2(int input){\n"
                "   return input+2\n"
                "}\n";
        QString result = CodeParser().parseCode(code);
        QCOMPARE(result, expected);
    }
};


QTEST_APPLESS_MAIN(TestParser)
#include "TestParser.moc"
