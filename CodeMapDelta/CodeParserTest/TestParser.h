#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include<QFile>
#include<QDir>
#include<QTextStream>

// Helper methods

QString readFileContent(const QString& filePath)
{
    QFile fileHandler(filePath);
    bool ok = fileHandler.open(QFile::ReadOnly | QFile::Text);
    if(!ok)
    {
        auto errorMessage = QStringLiteral("Failed to open file: %1").arg(filePath);
        FAIL(errorMessage.toStdString());
        return "";
    }
    QTextStream fileStream(&fileHandler);
    QString result = fileStream.readAll();
    return result;
}

QString getTestPatternPath(const QString& testName, const QString& fileName)
{
    QString current = QDir::currentPath();
    QString filePath = current + "/TestPatterns/" + testName + "/" + fileName;
    return filePath;
}

bool writeFileContent(const QString& filePath, const QString& content)
{
    QFile fileHandler(filePath);
    bool ok = fileHandler.open(QFile::WriteOnly);
    if(!ok)
    {
        auto errorMessage = QStringLiteral("Failed to open file: %1").arg(filePath);
        FAIL(errorMessage.toStdString());
        return false;
    }
    QTextStream fileStream(&fileHandler);
    fileStream << content;
    return true;
}

#endif // !TEST_HELPER_H