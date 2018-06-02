#include "codeparser.h"

#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/CompilerInstance.h"

/*
 * Preprocesses the input files and stores them in a string.
 * */
class PreprocessorEliminatorFrontendAction : public clang::PreprocessorFrontendAction
{
    std::string& preprocessedOutput;
public:

    PreprocessorEliminatorFrontendAction(std::string& preprocessedOutputDest) :
        preprocessedOutput(preprocessedOutputDest){}

    void ExecuteAction()
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


QString CodeParser::getPreprocessedCode(const QString& source)
{
    std::string result;
    PreprocessorEliminatorFrontendAction* ppe = new PreprocessorEliminatorFrontendAction(result);
    clang::tooling::runToolOnCode(ppe, source.toStdString());
    return QString::fromStdString(result);
}

QString CodeParser::getPreprocessedCodeFromPath(const QString& srcPath)
{
    std::string result;

    clang::tooling::FixedCompilationDatabase cdb("/", std::vector<std::string>());
    std::vector<std::string> src{ srcPath.toStdString() };
    clang::tooling::ClangTool tool(cdb, src);

    std::unique_ptr<clang::tooling::FrontendActionFactory> actionFactory = std::make_unique<PreprocessorFrontendActionFactory>(result);
    tool.run(actionFactory.get());
    return QString::fromStdString(result);
}
