
#include <QString>
#include <QJsonDocument>


void REQUIRE_JSON(const QString expected, const QString result)
{
    QJsonParseError err;
    auto& expJson = QJsonDocument::fromJson(expected.toLocal8Bit(), &err);
    INFO("Parsing expected json: " + err.errorString().toStdString() + " at position " + QString::number(err.offset).toStdString());
    auto& resJson = QJsonDocument::fromJson(result.toLocal8Bit(), &err);
    INFO("Parsing result json: " + err.errorString().toStdString() + " at position " + QString::number(err.offset).toStdString());
    REQUIRE(expJson == resJson);
}

// Custom Matcher

namespace Catch {
namespace Detail {

template<>
std::string stringify<QString>(const QString& e) {
    return e.toStdString();
}

template<>
std::string stringify<QJsonDocument>(const QJsonDocument& e) {
    return e.toJson().toStdString();
}

template<>
std::string stringify<QJsonParseError>(const QJsonParseError& e) {
    return e.errorString().toStdString();
}

}
}