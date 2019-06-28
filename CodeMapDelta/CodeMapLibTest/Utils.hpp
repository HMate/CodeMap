
#include <QString>
#include <QJsonDocument>

// Custom Matcher

namespace Catch {
namespace Detail {

template<>
std::string stringify<QString>(const QString& e) {
    return e.toStdString();
}

template<>
std::string stringify<QJsonDocument>(const QJsonDocument& e) {
    return e.toJson();
}

}
}