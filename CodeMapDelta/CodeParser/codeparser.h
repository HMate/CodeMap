#ifndef PARSER_H
#define PARSER_H

#include <QString>

class CodeParser
{
public:
    QString getPreprocessedCode(const QString& source);
    QString getPreprocessedCodeFromPath(const QString& srcPath);
};

#endif // PARSER_H
