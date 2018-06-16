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

    /* Test if preprocessor substitution is working*/
    void testPreprocessorWithFile()
    {
        QString codePath = getTestPatternPath("testPreprocessorWithFile", "test.cpp");
        QString expected = readFileContent(getTestPatternPath("testPreprocessorWithFile", "result.cpp"));

        QString result = CodeParser().getPreprocessedCodeFromPath(codePath);
        QCOMPARE(result, expected);
    }

    /* Test if preprocessor include substitution is working,
     * when the include is inside the same folder as the cpp*/
    void testPreprocessorInclude()
    {
        QString codePath = getTestPatternPath("testPreprocessorInclude", "test.cpp");
        QString expected = readFileContent(getTestPatternPath("testPreprocessorInclude", "result.cpp"));

        QString result = CodeParser().getPreprocessedCodeFromPath(codePath);
        QCOMPARE(result, expected);
    }

    /* Test if preprocessor include substitution is working,
     * when the include is outside of the cpp folder*/
    void testPreprocessorIncludeOuterDir()
    {
        QString codePath = getTestPatternPath("testPreprocessorIncludeOuterDir", "test.cpp");
        QString expected = readFileContent(getTestPatternPath("testPreprocessorIncludeOuterDir", "result.cpp"));

        QString result = CodeParser().getPreprocessedCodeFromPath(codePath, {getTestPatternPath("testPreprocessorIncludeOuterDir", "externalDependencies")});
        QCOMPARE(result, expected);
    }

    void testPreprocessorIncludeStdio()
    {
        QString codePath = getTestPatternPath("testPreprocessorIncludeStdio", "test.cpp");
        QString expected = readFileContent(getTestPatternPath("testPreprocessorIncludeStdio", "result.cpp"));
        writeFileContent(getTestPatternPath("testPreprocessorIncludeStdio", "out.cpp"), expected);

        QString result = CodeParser().getPreprocessedCodeFromPath(codePath);
        // Its complicated to actually compare the whole file content, because every define is expanded in stdio.h
        // just see if the method result contains the needed code + some stdio functions
        QVERIFY(result.contains(expected));
        QVERIFY(result.contains("__cdecl fopen"));
    }

    // Helper methods

    QString readFileContent(const QString& filePath)
    {
        QFile fileHandler(filePath);
        bool ok = fileHandler.open(QFile::ReadOnly | QFile::Text);
        if(!ok)
        {
            auto errorMessage = QStringLiteral("Failed to open file: %1").arg(filePath);
            qWarning() << errorMessage << endl;
            return "";
        }
        QTextStream fileStream(&fileHandler);
        QString result = fileStream.readAll();
        return result;
    }

    QString getTestPatternPath(const QString& testName, const QString& fileName)
    {
        QString current = QDir::currentPath();
        QString filePath = current+"/TestPatterns/"+testName+"/"+fileName;
        return filePath;
    }


    bool writeFileContent(const QString& filePath, const QString& content)
    {
        QFile fileHandler(filePath);
        bool ok = fileHandler.open(QFile::WriteOnly);
        if(!ok)
        {
            auto errorMessage = QStringLiteral("Failed to open file: %1").arg(filePath);
            qWarning() << errorMessage << endl;
            return false;
        }
        QTextStream fileStream(&fileHandler);
        fileStream << content;
        return true;
    }
};


QTEST_APPLESS_MAIN(TestParser)
#include "TestParser.moc"
