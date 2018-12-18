#include "codeparser.h"

#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/CompilerInstance.h"

#include <QStringList>
#include <QRegularExpression>

#include "parsererror.h"

namespace cm
{

bool ParserResult::hasErrors()
{
    return !errors.empty();
}

/*
clang -H: flag to list include dependencies
*/

/*
 * Preprocesses the input files and stores them in a string.
 * */
class PreprocessorEliminatorFrontendAction : public clang::PreprocessorFrontendAction
{
    std::string& preprocessedOutput;
public:

    PreprocessorEliminatorFrontendAction(std::string& preprocessedOutputDest) :
        preprocessedOutput(preprocessedOutputDest){}
    
    void ExecuteAction() override
    {
        clang::CompilerInstance &CI = getCompilerInstance();
        llvm::raw_string_ostream OS(preprocessedOutput);

        auto ppoo = CI.getPreprocessorOutputOpts();
        ppoo.ShowCPP = 1;
        ppoo.ShowComments = 1;

        /* Figured out how line markers work:
        first is line number where we enter the file, then file name.
        The flgas part is 1, if we entered the file thorught na import directive
        Flags is 2, if we entered back to a file by leaving an include.
        3 is when we enter a systems file. 
        4 is extern C? Not sure what that means right now and if i need to handle it.
        */
        ppoo.ShowLineMarkers = 1;
        ppoo.UseLineDirectives = 0;
        clang::DoPrintPreprocessedInput(CI.getPreprocessor(), &OS, ppoo);
        OS.flush();
    }
};

class PreprocessorFrontendActionFactory : public clang::tooling::FrontendActionFactory
{
    std::string& preprocessedOutput;
public:
    PreprocessorFrontendActionFactory(std::string& preprocessedOutDest) : preprocessedOutput(preprocessedOutDest){}

    clang::FrontendAction *create() override 
    { 
        return new PreprocessorEliminatorFrontendAction(preprocessedOutput); 
    }
};


QString CodeParser::getPreprocessedCode(const QString& source)
{
    std::string result;
    auto ppe = new PreprocessorEliminatorFrontendAction(result);
    clang::tooling::runToolOnCode(ppe, source.toStdString());
    return parseIncludes(QString::fromStdString(result)).code;
}

ParserResult CodeParser::getPreprocessedCodeFromPath(const QString& srcPath, const std::vector<QString>& includeDirs)
{
    std::vector<std::string> includes(includeDirs.size());
    for(auto& dir : includeDirs)
    {
        includes.emplace_back(QString("-I%1").arg(dir).toStdString());
    }

    clang::tooling::FixedCompilationDatabase cdb("/", includes);
    std::vector<std::string> src{ srcPath.toStdString() };
    clang::tooling::ClangTool tool(cdb, src);

    ParserErrorCollector errorCollector;
    tool.setDiagnosticConsumer(&errorCollector);
    
    std::string processedFile;
    std::unique_ptr<clang::tooling::FrontendActionFactory> actionFactory = std::make_unique<PreprocessorFrontendActionFactory>(processedFile);
    tool.run(actionFactory.get());
    
    ParserResult result;
    ParsedIncludes extracted = parseIncludes(QString::fromStdString(processedFile));
    result.code.content = extracted.code;
    result.includes = extracted.includes;
    auto errors = errorCollector.GetErrorsList();
    for (const auto& it : errors)
    {
        result.errors.emplace_back(QString::fromStdString(it));
    }
    return result;
}

enum class LineMarkerType
{
    Missing = 0,
    EnterInclude = 1,
    ExitInclude = 2,
    EnterSystemFile = 3,
    ExternC = 4,
    Invalid
};

struct LineMarker
{
    long enteringLine;
    QString filename;
    LineMarkerType type;
};

LineMarker parseLineMarker(const QString& line)
{
    LineMarker marker{-1, "", LineMarkerType::Invalid};

    // Example marker: # 1 "filename.h" 2
    QRegularExpression re("^#\\s*(\\d+)\\s*\\\"(.+)\\\"\\s*(\\d*)");
    QRegularExpressionMatch match = re.match(line);
    if(match.hasMatch()) {
        QString startLineString = match.captured(1);
        marker.enteringLine = startLineString.toLong();
        marker.filename = match.captured(2);
        QString typeString = match.captured(3);
        marker.type = LineMarkerType(typeString.toInt());
    }

    return marker;
}

ParsedIncludes CodeParser::parseIncludes(const QString& processed)
{
    ParsedIncludes result;

    auto lines = processed.split('\n');

    // TODO: include stack builder?

    static QStringList builtinFilenames = { "<built-in>", "<command line>" };

    IncludeSection section;

    bool isFirstLine = true;
    QString filename;
    size_t lineIndex = 1;
    for(QStringList::iterator line = lines.begin(); line != lines.end(); ) {
        if(line->startsWith("#")) {
            LineMarker marker = parseLineMarker(*line);
            if(isFirstLine)
            {
                filename = marker.filename;
                isFirstLine = false;
            }
            else if(!builtinFilenames.contains(marker.filename))
            {
                if(marker.type == LineMarkerType::EnterInclude)
                {
                    section.filename = marker.filename;
                    section.firstLine = lineIndex;
                }
                else if(marker.type == LineMarkerType::ExitInclude &&
                    section.filename != "")
                {
                    section.lastLine = lineIndex - 1;
                    result.includes.emplace_back(section);
                }
            }
            line = lines.erase(line);
        }
        else {
            ++line;
            ++lineIndex;
        }
    }

    result.code = lines.join("\n");
    return result;
}

}
