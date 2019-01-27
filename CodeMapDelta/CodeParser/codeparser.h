#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <QString>

namespace cm
{

typedef std::vector<QString> StringList;


struct IncludeSection
{
    QString filename;
    long firstLine;
    long lastLine;
};
typedef std::vector<IncludeSection> IncludeSectionList;

struct ParsedCodeFile
{
    QString content;
    IncludeSectionList includes;
};

struct ParserResult
{
    ParsedCodeFile code;
    StringList errors;

    bool hasErrors();
};

struct IncludeNode
{
    std::string name;
    std::string id;
    std::vector<IncludeNode> includes;
};

struct IncludeTree
{
    IncludeNode root;
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
    IncludeTree getIncludeTree(const QString& srcPath, const std::vector<QString>& includeDirs = std::vector<QString>());

    void parseAST(const QString& srcPath, const std::vector<QString>& includeDirs = std::vector<QString>());
};

}
#endif // PARSER_H
