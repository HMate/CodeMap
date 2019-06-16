
#include <QString>

// Custom Matcher

namespace Catch {
namespace Detail {

template<>
std::string stringify<QString>(const QString& e) {
    return e.toStdString();
}
}
}