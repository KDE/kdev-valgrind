/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
   Copyright 2016 Anton Anikin <anton@anikin.xyz>

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
#include <QUrl>

namespace Valgrind
{

namespace Cachegrind
{

Function* parseCachegrindItem(const QString& line, QStringList& eventsList)
{
    auto item = new Function;

    QStringList lineEventList = line.split(QChar(' '), QString::SkipEmptyParts);
    Q_ASSERT(lineEventList.size() >= eventsList.size());

    for (int i = 0; i < eventsList.size(); ++i) {
        item->eventValues += lineEventList.takeFirst().remove(QLatin1Char(',')).toInt();
    }

    QString fileCall = lineEventList.join(QChar(' '));
    int colonPosition = fileCall.indexOf(QChar(':'));
    if (colonPosition >= 0) {
        // file name
        auto fileUrl = QUrl::fromLocalFile(fileCall.mid(0, colonPosition)).adjusted(QUrl::NormalizePathSegments);
        item->fileNames += fileUrl.toLocalFile();

        // call name
        item->functionName = fileCall.mid(colonPosition + 1);
    }

    return item;
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
    QStringList eventsList;
    QString eventsString;
    QString line;

    QBuffer data(&baData);
    data.open(QIODevice::ReadOnly);

    while (!data.atEnd())
    {
        // remove useless characters
        line = data.readLine().simplified();

        if (parserState == ParseRoot) {
            if (line.startsWith(QLatin1String("Events shown:"))) {
                // 13 is 'Events shown:' length
                eventsString = line.mid(13).simplified();
                eventsList = eventsString.split(QChar(' '), QString::SkipEmptyParts);
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
            bool isTotal = (parserState == ParseProgramTotal);
            model->addItem(parseCachegrindItem(line, eventsList), isTotal);
            if (isTotal) {
                parserState = ParseProgramHeader;
            }
        }
    }

    model->setEventsList(eventsList);
}

}

}
