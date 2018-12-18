#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <QString>

namespace cm
{

typedef std::vector<QString> StringList;

struct ParsedCodeFile
{
    QString content;
};

struct IncludeSection
{
    QString filename;
    long firstLine;
    long lastLine;
};
typedef std::vector<IncludeSection> IncludeSectionList;

struct ParsedIncludes
{
    QString code;
    IncludeSectionList includes;
};

struct ParserResult
{
    ParsedCodeFile code;
    StringList errors;
    IncludeSectionList includes;

    bool hasErrors();
};


/// Contains methods to parse in cpp code files
class CodeParser
{
public:
    /* Returns the preprocessed code*/
    QString getPreprocessedCode(const QString& source);

    /* Returns the preprocessed content of the file.
     * includeDirs is a list of directory paths. They are searched for resolving includes.*/
    ParserResult getPreprocessedCodeFromPath(const QString& srcPath, const std::vector<QString>& includeDirs = std::vector<QString>());
private:
    ParsedIncludes parseIncludes(const QString& src);
};

}
#endif // PARSER_H
