#ifndef PARSERERROR_H
#define PARSERERROR_H

#include "clang/Basic/Diagnostic.h"

namespace cm{

class ParserErrorCollector : public clang::DiagnosticConsumer
{    
public:
    typedef std::vector<std::pair<clang::SourceLocation, std::string> > DiagList;

    /// HandleDiagnostic - Store the errors, warnings, and notes that are
    /// reported.
    void HandleDiagnostic(clang::DiagnosticsEngine::Level Level, const clang::Diagnostic &Info);
    DiagList GetErrorsList();

protected:
    DiagList Errors;
};

}
#endif // PARSERERROR_H