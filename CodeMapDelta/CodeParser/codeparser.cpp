#include "codeparser.h"

#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnosticBuffer.h"

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
        ppoo.ShowLineMarkers = 0;
        clang::DoPrintPreprocessedInput(CI.getPreprocessor(), &OS, ppoo);
        OS.flush();
    }
};

class PreprocessorFrontendActionFactory : public clang::tooling::FrontendActionFactory
{
    std::string& preprocessedOutput;
public:
    PreprocessorFrontendActionFactory(std::string& preprocessedOutDest) :
        preprocessedOutput(preprocessedOutDest){}
    clang::FrontendAction *create() override { return new PreprocessorEliminatorFrontendAction(preprocessedOutput); }
};

class MyDiagnosticConsumer : public clang::DiagnosticConsumer
{

};


QString CodeParser::getPreprocessedCode(const QString& source)
{
    std::string result;
    auto ppe = new PreprocessorEliminatorFrontendAction(result);
    clang::tooling::runToolOnCode(ppe, source.toStdString());
    return QString::fromStdString(result);
}

QString CodeParser::getPreprocessedCodeFromPath(const QString& srcPath)
{
    std::string result;

    clang::tooling::FixedCompilationDatabase cdb("/", std::vector<std::string>());
    std::vector<std::string> src{ srcPath.toStdString() };
    clang::tooling::ClangTool tool(cdb, src);

    // TODO: write MyDiagnosticConsumer to capture errors in format better suited to us
    clang::TextDiagnosticBuffer diagnostics;
    tool.setDiagnosticConsumer(&diagnostics);

    std::unique_ptr<clang::tooling::FrontendActionFactory> actionFactory = std::make_unique<PreprocessorFrontendActionFactory>(result);
    tool.run(actionFactory.get());
    for (auto it = diagnostics.err_begin(), ie = diagnostics.err_end(); it != ie; ++it)
    {
        auto& loc = it->first;
    }
    for (auto it = diagnostics.note_begin(), ie = diagnostics.note_end(); it != ie; ++it)
    {
        auto& loc = it->first;
    }
    for (auto it = diagnostics.warn_end(), ie = diagnostics.warn_end(); it != ie; ++it)
    {
        auto& loc = it->first;
    }
    for (auto it = diagnostics.remark_begin(), ie = diagnostics.remark_end(); it != ie; ++it)
    {
        auto& loc = it->first;
    }
    return QString::fromStdString(result);
}

QString CodeParser::getPreprocessedCodeFromPath(const QString& srcPath, const std::vector<QString>& includeDirs)
{
    std::string result;
    std::vector<std::string> includes;
    includes.reserve(includeDirs.size());

    for(auto& dir : includeDirs)
    {
        includes.emplace_back(QString("-I%1").arg(dir).toStdString());
    }

    clang::tooling::FixedCompilationDatabase cdb("/", includes);
    std::vector<std::string> src{ srcPath.toStdString() };
    clang::tooling::ClangTool tool(cdb, src);

    std::unique_ptr<clang::tooling::FrontendActionFactory> actionFactory = std::make_unique<PreprocessorFrontendActionFactory>(result);
    tool.run(actionFactory.get());
    return QString::fromStdString(result);
}
