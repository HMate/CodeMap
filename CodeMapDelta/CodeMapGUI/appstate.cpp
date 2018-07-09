#include "appstate.h"

#include "filesystem.h"
#include "QDebug"

const QString STATE_FILE_NAME = "state.cms";

AppStateHandler::AppStateHandler()
{
    dataDirPath = FS::concat(FS::getCWD(), "data");
}

void AppStateHandler::addFileView(const QString& filePath)
{
    fileViews.append(filePath);
}

const QStringList& AppStateHandler::getFileViews()
{
    return fileViews;
}

QString serialize(const QStringList& strings)
{
    QString result = strings.join('\n');
    return result;
}

void AppStateHandler::saveStateToDisk()
{
    QString stateFilePath = FS::concat(dataDirPath, STATE_FILE_NAME);
    QString version = "1\n";
    QString state = serialize(fileViews);
    bool succeed = FS::saveFile(stateFilePath, version+state);
    qDebug() << "Writing state result: " << succeed << "\n" << (version+state);
}

void AppStateHandler::loadStateFromDisk()
{
    QString stateFilePath = FS::concat(dataDirPath, STATE_FILE_NAME);
    QString state = FS::readFile(stateFilePath);
    qDebug() << "Loading state: \n"<< state;
    QStringList parts = state.split('\n', QString::SkipEmptyParts);
    fileViews = parts.mid(1);
}
