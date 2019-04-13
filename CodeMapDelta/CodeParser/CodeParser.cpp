#include "CodeParser.h"

#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/CompilerInstance.h"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTUnit.h"


#include "Utils.h"
#include "ParserError.h"
#include "LineMarkers.h"
#include "IncludeTreeBuilder.h"

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
        ppoo.ShowIncludeDirectives = 1;
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
    std::string processedFile;
    auto ppe = new PreprocessorEliminatorFrontendAction(processedFile);
    clang::tooling::runToolOnCode(ppe, source.toStdString());
    return parseLineMarkers(QString::fromStdString(processedFile)).content;
}

ParserResult CodeParser::getPreprocessedCodeFromPath(const QString& srcPath, const std::vector<QString>& includeDirs)
{
    clang::tooling::FixedCompilationDatabase cdb = createCompilationDatabase(srcPath, includeDirs);
    std::vector<std::string> src{ srcPath.toStdString() };
    clang::tooling::ClangTool tool(cdb, src);

    ParserErrorCollector errorCollector;
    tool.setDiagnosticConsumer(&errorCollector);
    
    std::string processedFile;
    std::unique_ptr<clang::tooling::FrontendActionFactory> actionFactory = std::make_unique<PreprocessorFrontendActionFactory>(processedFile);
    tool.run(actionFactory.get());
    
    ParserResult result;
    ParsedCodeFile extracted = parseLineMarkers(QString::fromStdString(processedFile));
    result.code = extracted;
    auto errors = errorCollector.GetErrorsList();
    for (const auto& it : errors)
    {
        result.errors.emplace_back(QString::fromStdString(it));
    }
    return result;
}


class SuperVisitor : public clang::RecursiveASTVisitor<SuperVisitor> 
{
    struct Node
    {
        std::string name;
        unsigned int row;
        unsigned int col;
    };
public:
    std::vector<Node> VisitedNodes;

    bool VisitVarDecl(clang::VarDecl* D) {
        auto& ctx = D->getASTContext();
        auto& sm = ctx.getSourceManager();
        auto& location = ctx.getFullLoc(D->getLocation());
        
        Node n{ D->getNameAsString(), location.getSpellingLineNumber(), location.getSpellingColumnNumber() };
        VisitedNodes.push_back(n);
        return true;
    }
};

void CodeParser::parseAST(const QString& srcPath, const std::vector<QString>& includeDirs)
{
    clang::tooling::FixedCompilationDatabase cdb = createCompilationDatabase(srcPath, includeDirs);
    std::vector<std::string> src{ srcPath.toStdString() };
    clang::tooling::ClangTool tool(cdb, src);

    ParserErrorCollector errorCollector;
    tool.setDiagnosticConsumer(&errorCollector);
    
    std::vector<std::unique_ptr<clang::ASTUnit>> Asts;
    tool.buildASTs(Asts);
    
    SuperVisitor v;
    v.TraverseTranslationUnitDecl(Asts[0]->getASTContext().getTranslationUnitDecl());

    ParserResult result;
}



std::unique_ptr<IncludeTree> CodeParser::getIncludeTree(const QString& srcPath, const std::vector<QString>& includeDirs)
{
    return cm::getIncludeTree(srcPath, includeDirs);
}

}
