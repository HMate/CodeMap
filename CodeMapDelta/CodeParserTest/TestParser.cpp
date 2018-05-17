#include <QtTest>

#include "codeparser.h"

class TestParser : public QObject
{
    Q_OBJECT
private slots:
    void testPreprocesssorForString()
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
        QString result = CodeParser().getPreprocessedCode(code);
        QCOMPARE(result, expected);
    }

    void testPreprocessorWithFile()
    {
        QString code = readFileContent("testPreprocessorWithFile", "test.cpp");
        QString expected = readFileContent("testPreprocessorWithFile", "result.cpp");

        QString result = CodeParser().getPreprocessedCode(code);
        QCOMPARE(result, expected);
    }

    QString readFileContent(const QString& testName, const QString& fileName)
    {
        QString current = QDir::currentPath();
        QString filePath = current+"/TestPatterns/"+testName+"/"+fileName;
        QFile fileHandler(filePath);
        bool ok = fileHandler.open(QFile::ReadOnly | QFile::Text);
        QTextStream fileStream(&fileHandler);
        QString result = fileStream.readAll();
        return result;
    }
};


QTEST_APPLESS_MAIN(TestParser)
#include "TestParser.moc"
