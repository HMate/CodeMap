#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include "clang/Tooling/Tooling.h"

namespace cm
{

clang::tooling::FixedCompilationDatabase createCompilationDatabase(const QString& srcPath, const std::vector<QString>& includeDirs = std::vector<QString>());

}
#endif // UTILS_H