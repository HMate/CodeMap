#include "appstate.h"

#include <QDebug>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "common_types.h"
#include "filesystem.h"

const QString STATE_FILE_NAME = "state.cms";

AppStateHandler::AppStateHandler()
{
    dataDirPath = FS::concat(FS::getCWD(), "data");
}

void AppStateHandler::addFileView(const QString& filePath)
{
    fileViews.append(filePath);
}

void AppStateHandler::addFileView(const QString& filePath, long long tabViewIndex)
{
    // TODO save the tabViewIndex to the state
    fileViews.append(filePath);
}

void AppStateHandler::removeFileView(const QString& filePath)
{
    for(int i = 0; i < fileViews.length(); i++)
    {
        if(fileViews[i] == filePath)
        {
            fileViews.erase(fileViews.begin()+i);
            break;
        }
    }
}

void AppStateHandler::removeFileView(const QString& filePath, long long tabViewIndex)
{
    for(int i = 0; i < fileViews.length(); i++)
    {
        if(fileViews[i] == filePath)
        {
            fileViews.erase(fileViews.begin()+i);
            break;
        }
    }
}

const QStringList& AppStateHandler::getFileViews()
{
    return fileViews;
}

const QString& AppStateHandler::getLastOpenedDirectory()
{
    return lastOpenedDirPath;
}

void AppStateHandler::setLastOpenedDirectory(const QString& path)
{
    lastOpenedDirPath = path;
}

void AppStateHandler::saveStateToDisk()
{
    QString stateFilePath = FS::concat(dataDirPath, STATE_FILE_NAME);

    QJsonObject stateJson{{"version", 2}};
    QJsonArray fileViewsJson;
    for(auto& f : fileViews)
    {
        fileViewsJson.push_back(f);
    }
    stateJson.insert("fileViews", fileViewsJson);

    const QString serialized = QJsonDocument(stateJson).toJson();
    bool succeed = FS::saveFile(stateFilePath, serialized);
    qDebug() << "Writing state result: " << succeed << "\n" << serialized;
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
            const QJsonValue& fileViewsValueRef = stateJson["fileViews"];
            if(fileViewsValueRef.isArray())
            {
                QJsonArray fileViewsJson = fileViewsValueRef.toArray();
                fileViews.clear();
                for(const auto f : fileViewsJson)
                {
                    if(f.isString())
                        fileViews.append(f.toString());
                }
            }
        }
    }
    else
    {
        qDebug() << "Failed to load app state!";
    }
}
