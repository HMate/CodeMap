#include <QtTest>

#include "codeparser.h"

class TestParser : public QObject
{
    Q_OBJECT
private slots:
    void testFoo()
    {
        QString asd = CodeParser().parseCode("asd");
        QCOMPARE(asd, "asd");
    }
};


QTEST_APPLESS_MAIN(TestParser)
#include "TestParser.moc"
