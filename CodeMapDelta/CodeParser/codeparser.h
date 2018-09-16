#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <QString>

namespace cm
{

struct ParserResult
{
    typedef std::vector<QString> ErrorList;

    QString content;
    ErrorList errors;

    bool hasErrors();
};

class CodeParser
{
public:
    /* Returns the preprocessed code*/
    QString getPreprocessedCode(const QString& source);

    /* Returns the preprocessed content of the file.
     * includeDirs is a list of directory paths. They are searched for resolving includes.*/
    ParserResult getPreprocessedCodeFromPath(const QString& srcPath, const std::vector<QString>& includeDirs = std::vector<QString>());
};

}
#endif // PARSER_H
