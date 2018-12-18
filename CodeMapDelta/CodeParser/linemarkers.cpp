#include "linemarkers.h"


#include <QStringList>
#include <QRegularExpression>

namespace cm
{

enum class LineMarkerType
{
    Missing = 0,
    EnterInclude = 1,
    ExitInclude = 2,
    EnterSystemFile = 3,
    ExternC = 4,
    Invalid
};

struct LineMarker
{
    long enteringLine;
    QString filename;
    LineMarkerType type;
};

LineMarker parseLineMarker(const QString& line)
{
    LineMarker marker{ -1, "", LineMarkerType::Invalid };

    // Example marker: 
    //# 1 "filename.h" 2
    QRegularExpression re("^#\\s*(\\d+)\\s*\\\"(.+)\\\"\\s*(\\d*)");
    QRegularExpressionMatch match = re.match(line);
    if(match.hasMatch()) {
        QString startLineString = match.captured(1);
        marker.enteringLine = startLineString.toLong();
        marker.filename = match.captured(2);
        QString typeString = match.captured(3);
        marker.type = LineMarkerType(typeString.toInt());
    }

    return marker;
}

QString parseInclude(QString& line)
{
    QString result;

    // Example include: 
    //#include "filename.h" /* clang -E -dH */
    QRegularExpression re("^#include\\s*[<\"](.+)[>\"]\\s+\\/\\*");
    QRegularExpressionMatch match = re.match(line);
    if(match.hasMatch()) {
        result = match.captured(1);

        // we dont want the clang comment at the end
        auto index = line.indexOf(QRegularExpression("\\s+/\\*"));
        line.truncate(index);
    }

    return result;
}

ParsedCodeFile parseLineMarkers(const QString& processed)
{
    ParsedCodeFile result;

    auto lines = processed.split('\n');

    // TODO: include stack builder?

    static QStringList builtinFilenames = { "<built-in>", "<command line>" };

    IncludeSection section;

    bool isFirstLine = true;
    QString filename;
    size_t lineIndex = 1;
    for(QStringList::iterator line = lines.begin(); line != lines.end(); ) {
        if(line->startsWith("#")) {
            LineMarker marker = parseLineMarker(*line);
            if(marker.type != LineMarkerType::Invalid)
            {
                if(isFirstLine)
                {
                    filename = marker.filename;
                    isFirstLine = false;
                }
                else if(!builtinFilenames.contains(marker.filename))
                {
                    if(marker.type == LineMarkerType::EnterInclude)
                    {
                    }
                    else if(marker.type == LineMarkerType::ExitInclude &&
                        section.filename != "")
                    {
                        section.lastLine = lineIndex - 1;
                        result.includes.emplace_back(section);
                    }
                }
                line = lines.erase(line);
            }
            else
            {
                // line is include directive
                section.filename = parseInclude(*line);
                section.firstLine = lineIndex;
                ++line;
                ++lineIndex;
            }
        }
        else {
            ++line;
            ++lineIndex;
        }
    }

    result.content = lines.join("\n");
    return result;
}

}