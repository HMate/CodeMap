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

Settings& AppStateHandler::settings()
{
	return appSettings;
}

const QString& AppStateHandler::getLastOpenedDirectory()
{
    return lastOpenedDirPath;
}

void AppStateHandler::setLastOpenedDirectory(const QString& path)
{
    lastOpenedDirPath = path;
}

QJsonArray json(QStringList list)
{
	QJsonArray result;
	for (auto& s : list)
	{
		result.push_back(s);
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
    QJsonArray fileViewsJson = json(fileViews);
    stateJson.insert("fileViews", fileViewsJson);
	QJsonObject settingsJson = json(appSettings);
	stateJson.insert("settings", settingsJson);

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

			const QJsonValue& settingsValueRef = stateJson["settings"];
			if (settingsValueRef.isObject())
			{
				QJsonObject settingsJson = settingsValueRef.toObject();
				const auto& includesValueRef = settingsJson["globalIncludes"];
				if (includesValueRef.isArray())
				{
					QJsonArray includesJson = includesValueRef.toArray();
					appSettings.globalIncludes.clear();
					for (const auto f : includesJson)
					{
						if (f.isString())
							appSettings.globalIncludes.append(f.toString());
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
