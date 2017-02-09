/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
   Copyright 2016 Anton Anikin <anton.anikin@htower.ru>

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

#include <klocalizedstring.h>

#include <QBuffer>

namespace Valgrind
{

namespace Cachegrind
{

Parser::Parser(QObject* parent)
    : QObject(parent)
    , m_model(nullptr)
{
}

Parser::~Parser()
{
}

void Parser::parseCachegrindItem(const QString& line, bool programTotal)
{
    auto item = new Function;

    QStringList lineEventList = line.split(QChar(' '), QString::SkipEmptyParts);
    Q_ASSERT(lineEventList.size() >= m_eventsList.size());

    for (int i = 0; i < m_eventsList.size(); ++i) {
        item->eventValues += lineEventList.takeFirst().remove(",").toInt();
    }

    QString fileCall = lineEventList.join(QChar(' '));
    int colonPosition = fileCall.indexOf(QChar(':'));
    if (colonPosition >= 0) {
        // file name
        item->fileNames += fileCall.mid(0, colonPosition);

        // call name
        item->functionName = fileCall.mid(colonPosition + 1);
    }

    m_model->addItem(item, programTotal);
}

enum CachegrindParserState
{
    ParseRootModel,
    ParseProgramTotalHeader,
    ParseProgramTotal,
    ParseProgramHeader,
    ParseProgram
};

void Parser::parse(QByteArray& baData, FunctionsModel* model)
{
    Q_ASSERT(model);
    m_model = model;

    CachegrindParserState parserState = ParseRootModel;
    QString eventsString;
    QString line;

    QBuffer data(&baData);
    data.open(QIODevice::ReadOnly);

    while (!data.atEnd())
    {
        // remove useless characters
        line = data.readLine().simplified();

        if (parserState == ParseRootModel) {
            if (line.startsWith("Events shown:")) {
                // 13 is 'Events shown:' length
                eventsString = line.mid(13).simplified();
                m_eventsList = eventsString.split(QChar(' '), QString::SkipEmptyParts);
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
                parseCachegrindItem(line, true);
                parserState = ParseProgramHeader;
            }
            else
                parseCachegrindItem(line, false);
        }
    }

    model->setEventsList(m_eventsList);
    m_model = nullptr;
}

}

}
