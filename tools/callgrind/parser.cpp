/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
   Copyright 2016-2017 Anton Anikin <anton.anikin@htower.ru>

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

namespace valgrind
{

namespace Callgrind
{

Parser::Parser()
    : m_model(nullptr)
    , m_caller(nullptr)
{
}

Parser::~Parser()
{
}

void Parser::parseCallInformation(const QString& line, bool programTotal)
{
    static const QRegularExpression binaryExpression("^(.*)\\[(.*)\\]$");
    static const QRegularExpression callCountExpression("^(.*)\\((\\d+)x\\)$");

    QStringList lineItems = line.split(QChar(' '), QString::SkipEmptyParts);
    for (int i = 0; i < m_eventTypes.size(); ++i) {
        lineItems[i].remove(",");
    }

    if (programTotal) {
        while (lineItems.size() > m_eventTypes.size()) {
            lineItems.removeLast();
        }
        m_model->setEventTotals(lineItems);

        return;
    }

    const char lineType = lineItems.takeAt(m_eventTypes.size()).at(0).toLatin1();

    // skip caller lines
    if (lineType == '<') {
        return;
    }

    QString idString;
    while (lineItems.size() > m_eventTypes.size()) {
        idString += lineItems.takeAt(m_eventTypes.size()) + ' ';
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

    QString sourceFile = idString.mid(0, colonPos);
    QString functionName = idString.mid(colonPos + 1);

    auto function = m_model->addFunction(functionName, sourceFile, binaryFile);

    // the function itself
    if (lineType == '*') {
        m_caller = function;
        m_caller->addEventValues(lineItems);
    }

    // the callee
    else if (lineType == '>') {
        m_model->addCall(m_caller, function, callCount, lineItems);
    }

    else {
        qCWarning(KDEV_VALGRIND) << "unknown line type:" << lineType;
    }
}

enum ParserState
{
    Root,
    ProgramTotalHeader,
    ProgramTotal,
    ProgramHeader,
    Program
};

void Parser::parse(QByteArray& baData, FunctionsModel* model)
{
    Q_ASSERT(model);
    m_model = model;

    ParserState parserState = Root;
    QString eventsString;
    QString line;

    QBuffer data(&baData);
    data.open(QIODevice::ReadOnly);

    while (!data.atEnd()) {
        line = data.readLine().simplified();

        if (line.startsWith("--") && line.contains("annotated source:")) {
                break;
        }

        if (parserState == Root) {
            if (line.startsWith("Events shown:")) {
                // 13 is 'Events shown:' size;
                eventsString = line.mid(13).simplified();

                m_eventTypes = eventsString.split(QChar(' '), QString::SkipEmptyParts);
                m_model->setEventTypes(m_eventTypes);

                parserState = ProgramTotalHeader;
            }
        }

        else if (parserState == ProgramTotalHeader) {
            if (line == eventsString) {
                parserState = ProgramTotal;
            }
        }

        else if (parserState == ProgramHeader) {
            if (line.startsWith(eventsString)) {
                parserState = Program;
            }
        }

        else if (!line.isEmpty() && line.at(0).isDigit()) {
            if (parserState == ProgramTotal) {
                parseCallInformation(line, true);
                parserState = ProgramHeader;
            }
            else {
                parseCallInformation(line, false);
            }
        }
    }

    m_model = nullptr;
}

}

}
