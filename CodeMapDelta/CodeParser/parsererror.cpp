#include "parsererror.h"

#include "llvm/ADT/SmallString.h"
#include "clang/Basic/SourceManager.h"

namespace cm{

void ParserErrorCollector::HandleDiagnostic(clang::DiagnosticsEngine::Level Level,
                                            const clang::Diagnostic &Info) {
    // Default implementation (Warnings/errors count).
    DiagnosticConsumer::HandleDiagnostic(Level, Info);

    clang::SmallString<100> Buf;
    Info.FormatDiagnostic(Buf);

    auto& loc = Info.getLocation();

    if(Info.hasSourceManager())
    {
        auto& sm = Info.getSourceManager();
        auto presumed = sm.getPresumedLoc(loc);
        auto nameRef = sm.getFilename(loc);
    }

    switch (Level) {
        default: llvm_unreachable("Diagnostic not handled during diagnostic buffering!");
        case clang::DiagnosticsEngine::Note:
        case clang::DiagnosticsEngine::Warning:
        case clang::DiagnosticsEngine::Remark:
        case clang::DiagnosticsEngine::Error:
        case clang::DiagnosticsEngine::Fatal:
            Errors.emplace_back(loc, Buf.str());
        break;
    }
    int debug =3;
}

ParserErrorCollector::DiagList ParserErrorCollector::GetErrorsList()
{
    return Errors;
}

}