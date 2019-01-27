#include "utils.h"


namespace cm
{

clang::tooling::FixedCompilationDatabase createCompilationDatabase(const QString& srcPath, const std::vector<QString>& includeDirs)
{
    std::vector<std::string> includes(includeDirs.size());
    for(auto& dir : includeDirs)
    {
        includes.emplace_back(QString("-I%1").arg(dir).toStdString());
    }

    clang::tooling::FixedCompilationDatabase cdb("/", includes);
    return cdb;
}

}
