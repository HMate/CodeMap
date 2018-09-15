#include "parsererror.h"

#include "llvm/Support/FormatVariadic.h"

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
    
    const char* filename;
    uint32_t line;
    uint32_t column;
    if(Info.hasSourceManager())
    {
        auto& sm = Info.getSourceManager();
        auto presumed = sm.getPresumedLoc(loc);

        filename = presumed.getFilename();
        line = presumed.getLine();
        column = presumed.getColumn();
    }

    char* level;
    switch (Level) {
        default: llvm_unreachable("Diagnostic not handled during diagnostic buffering!");
        case clang::DiagnosticsEngine::Note: level = "Note"; break;
        case clang::DiagnosticsEngine::Warning: level = "Warning"; break;
        case clang::DiagnosticsEngine::Remark: level = "Remark"; break;
        case clang::DiagnosticsEngine::Error: level = "Error"; break;
        case clang::DiagnosticsEngine::Fatal: level = "Fatal"; break;
        break;
    }
    
    auto finalString = llvm::formatv("Location {0} {1}:{2} | [{3}] {4}", filename, line, column, level, Buf);
    Errors.emplace_back(finalString.str());
}

ParserErrorCollector::ErrorList ParserErrorCollector::GetErrorsList()
{
    return Errors;
}

}