#include "AppState.h"

#include <QDebug>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "CommonTypes.h"
#include "FileSystem.h"

const QString STATE_FILE_NAME = "state.cms";

AppStateHandler::AppStateHandler()
{
    dataDirPath = FS::getDataFolder();
}

void AppStateHandler::addFileView(const QString& filePath, long long tabIndex)
{
    m_fileViews.emplace_back(filePath, tabIndex);
}

void AppStateHandler::removeFileView(const QString& filePath, long long tabIndex)
{
    for(int i = 0; i < m_fileViews.size(); i++)
    {
        auto& item = m_fileViews[i];
        if(item.path == filePath &&
            item.tabIndex == tabIndex)
        {
            m_fileViews.erase(m_fileViews.begin()+i);
            break;
        }
    }
}

const std::vector<FileViewStateItem>& AppStateHandler::getFileViews()
{
    return m_fileViews;
}

Settings& AppStateHandler::settings()
{
    return m_appSettings;
}

const QString& AppStateHandler::getLastOpenedDirectory()
{
    return lastOpenedDirPath;
}

void AppStateHandler::setLastOpenedDirectory(const QString& path)
{
    lastOpenedDirPath = path;
}

QJsonArray json(const QStringList& list)
{
    QJsonArray result;
    for (auto& s : list)
    {
        result.push_back(s);
    }
    return result;
}

QJsonArray json(const std::vector<FileViewStateItem>& list)
{
    QJsonArray result;
    for (auto& s : list)
    {
        QJsonObject o{ {"path", s.path}, {"index", s.tabIndex} };
        result.push_back(o);
    }
    return result;
}

QJsonObject json(Settings settings)
{
    QJsonObject result;
    result.insert("globalIncludes", json(settings.globalIncludes));
    return result;
}

void AppStateHandler::saveStateToDisk()
{
    QString stateFilePath = FS::concat(dataDirPath, STATE_FILE_NAME);

    QJsonObject stateJson{{"version", 2}};
    QJsonArray fileViewsJson = json(m_fileViews);
    stateJson.insert("fileViews", fileViewsJson);
    QJsonObject settingsJson = json(m_appSettings);
    stateJson.insert("settings", settingsJson);

    const QString serialized = QJsonDocument(stateJson).toJson();
    bool succeed = FS::saveFile(stateFilePath, serialized);
    qDebug() << "Writing state result: " << succeed << "\n" << serialized;
}

std::vector<FileViewStateItem> loadFileViewJson(const QJsonValue& fileViewsValueRef)
{
    std::vector<FileViewStateItem> result;
    if (fileViewsValueRef.isArray())
    {
        QJsonArray fileViewsJson = fileViewsValueRef.toArray();
        for (const auto f : fileViewsJson)
        {
            if (f.isObject())
            {
                auto& itemJson = f.toObject();
                QJsonValue pathVal = itemJson["path"];
                if (pathVal.isString())
                {
                    long long index = 0;
                    QJsonValue indexVal = itemJson["index"];
                    if (indexVal.isDouble())
                        index = indexVal.toInt();

                    result.emplace_back(pathVal.toString(), index);
                }
            }
        }
    }
    return result;
}

void AppStateHandler::loadStateFromDisk()
{
    QString stateFilePath = FS::concat(dataDirPath, STATE_FILE_NAME);
    QString state = FS::readFile(stateFilePath);
    qDebug() << "Loading state: \n"<< state;

    // TODO: error handling, input validation
    QJsonDocument doc = QJsonDocument::fromJson(state.toLocal8Bit());
    if(!doc.isNull() && doc.isObject())
    {
        const auto& stateJson = doc.object();
        const QJsonValue& versionJson = stateJson["version"];
        if(versionJson != QJsonValue::Undefined && versionJson.isDouble())
        {
            int version = versionJson.toInt();
            if(version != 2)
                qDebug() << "State version is unknown: "<< state << "!";
            m_fileViews = loadFileViewJson(stateJson["fileViews"]);

            const QJsonValue& settingsValueRef = stateJson["settings"];
            if (settingsValueRef.isObject())
            {
                QJsonObject settingsJson = settingsValueRef.toObject();
                const auto& includesValueRef = settingsJson["globalIncludes"];
                if (includesValueRef.isArray())
                {
                    QJsonArray includesJson = includesValueRef.toArray();
                    m_appSettings.globalIncludes.clear();
                    for (const auto f : includesJson)
                    {
                        if (f.isString())
                            m_appSettings.globalIncludes.append(f.toString());
                    }
                }
            }
        }
    }
    else
    {
        qDebug() << "Failed to load app state!";
    }
}
