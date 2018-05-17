#ifndef PARSER_H
#define PARSER_H

#include <QString>

class CodeParser
{
public:
    QString getPreprocessedCode(const QString& source);
};

#endif // PARSER_H
