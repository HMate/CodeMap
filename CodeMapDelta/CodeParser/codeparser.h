#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <QString>

namespace cm
{

class CodeParser
{
public:
    /* Returns the preprocessed code*/
    QString getPreprocessedCode(const QString& source);

    /* Returns the preprocessed content of the file */
    QString getPreprocessedCodeFromPath(const QString& srcPath);

    /* Returns the preprocessed content of the file.
     * includeDirs is a list of directory paths. They are searched for resolving includes.*/
    QString getPreprocessedCodeFromPath(const QString& srcPath, const std::vector<QString>& includeDirs);
};

}
#endif // PARSER_H
