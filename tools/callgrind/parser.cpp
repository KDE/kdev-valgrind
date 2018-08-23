/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
   Copyright 2016-2017 Anton Anikin <anton@anikin.xyz>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "parser.h"

#include "debug.h"
#include "model.h"

#include <QBuffer>
#include <QRegularExpression>
#include <QUrl>

namespace Valgrind
{

namespace Callgrind
{

void parseCallInformation(
    const QString& line,
    bool programTotal,
    const QStringList& eventTypes,
    Function*& caller,
    FunctionsModel* model)
{
    static const QRegularExpression binaryExpression("^(.*)\\[(.*)\\]$");
    static const QRegularExpression callCountExpression("^(.*)\\((\\d+)x\\)$");

    const int eventsCount = eventTypes.size();

    QStringList lineItems = line.split(QChar(' '), QString::SkipEmptyParts);
    for (int i = 0; i < eventsCount; ++i) {
        lineItems[i].remove(QLatin1Char(','));
    }

    if (programTotal) {
        while (lineItems.size() > eventsCount) {
            lineItems.removeLast();
        }
        model->setEventTotals(lineItems);

        return;
    }

    const char lineType = lineItems.takeAt(eventsCount).at(0).toLatin1();

    // skip caller lines
    if (lineType == '<') {
        return;
    }

    QString idString;
    while (lineItems.size() > eventsCount) {
        idString += QStringLiteral("%1 ").arg(lineItems.at(eventsCount));
        lineItems.removeAt(eventsCount);
    }
    idString = idString.trimmed();

    QString binaryFile;

    auto match = binaryExpression.match(idString);
    if (match.hasMatch()) {
        binaryFile = match.captured(2).trimmed();
        idString = match.captured(1).trimmed();
    }

    int callCount = 0;
    match = callCountExpression.match(idString);
    if (match.hasMatch()) {
        callCount = match.captured(2).toInt();
        idString = match.captured(1).trimmed();
    }

    int colonPos = idString.indexOf(':');
    Q_ASSERT(colonPos >= 0);

    auto sourceUrl = QUrl::fromLocalFile(idString.mid(0, colonPos)).adjusted(QUrl::NormalizePathSegments);
    QString sourceFile = sourceUrl.toLocalFile();
    QString functionName = idString.mid(colonPos + 1);

    auto function = model->addFunction(functionName, sourceFile, binaryFile);

    // the function itself
    if (lineType == '*') {
        caller = function;
        caller->addEventValues(lineItems);
    }

    // the callee
    else if (lineType == '>') {
        model->addCall(caller, function, callCount, lineItems);
    }

    else {
        qCWarning(KDEV_VALGRIND) << "unknown line type:" << lineType;
    }
}

enum ParserState
{
    ParseRoot,
    ParseProgramTotalHeader,
    ParseProgramTotal,
    ParseProgramHeader,
    ParseProgram
};

void parse(QByteArray& baData, FunctionsModel* model)
{
    Q_ASSERT(model);

    ParserState parserState = ParseRoot;
    QStringList eventTypes;
    QString eventsString;
    QString line;

    Function* caller = nullptr;

    QBuffer data(&baData);
    data.open(QIODevice::ReadOnly);

    while (!data.atEnd()) {
        line = data.readLine().simplified();

        if (line.startsWith(QLatin1String("--")) && line.contains(QLatin1String("annotated source:"))) {
                break;
        }

        if (parserState == ParseRoot) {
            if (line.startsWith(QLatin1String("Events shown:"))) {
                // 13 is 'Events shown:' size;
                eventsString = line.mid(13).simplified();

                eventTypes = eventsString.split(QChar(' '), QString::SkipEmptyParts);
                model->setEventTypes(eventTypes);

                parserState = ParseProgramTotalHeader;
            }
        }

        else if (parserState == ParseProgramTotalHeader) {
            if (line == eventsString) {
                parserState = ParseProgramTotal;
            }
        }

        else if (parserState == ParseProgramHeader) {
            if (line.startsWith(eventsString)) {
                parserState = ParseProgram;
            }
        }

        else if (!line.isEmpty() && line.at(0).isDigit()) {
            if (parserState == ParseProgramTotal) {
                parseCallInformation(line, true, eventTypes, caller, model);
                parserState = ParseProgramHeader;
            }
            else {
                parseCallInformation(line, false, eventTypes, caller, model);
            }
        }
    }
}

}

}
