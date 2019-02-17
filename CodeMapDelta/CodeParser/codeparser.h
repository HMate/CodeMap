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

struct IncludeTree;

class IncludeNodeRef
{
    IncludeTree& tree;
    bool fullInclude;
public:
    size_t index;

    IncludeNodeRef(IncludeTree& tree, size_t index, bool fullInclude);
    IncludeNodeRef& operator=(const IncludeNodeRef& o);
    void addInclude(IncludeTree& tree, size_t index, bool fullInclude);

    const std::string name() const;
    const std::string path() const;
    const std::vector<IncludeNodeRef>& includes() const;
    // Returns false if the include is skipped due to header guard
    void setFullInclude(bool fullInclude);
    bool isFullInclude() const;
    bool isRecursive() const;
};

struct IncludeNode
{
    const std::string name;
    const std::string path;
    std::vector<IncludeNodeRef> includes;

    IncludeNode(std::string name, std::string path) : name(name), path(path){}
};

struct IncludeTree
{
    std::vector<IncludeNode> nodes;

    IncludeNodeRef root();
    IncludeNode& node(const IncludeNodeRef& ref);
    const IncludeNode& node(const IncludeNodeRef& ref) const;
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
    std::unique_ptr<IncludeTree> getIncludeTree(const QString& srcPath, const std::vector<QString>& includeDirs = std::vector<QString>());

    void parseAST(const QString& srcPath, const std::vector<QString>& includeDirs = std::vector<QString>());
};

}
#endif // PARSER_H
