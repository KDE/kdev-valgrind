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
#include "modelitem.h"

#include <QBuffer>
#include <QRegularExpression>

namespace valgrind
{

CallgrindParser::CallgrindParser()
    : m_model(nullptr)
    , m_lastCall(nullptr)
    , m_numCalls(0)
    , m_totalCountItem(nullptr)
{
}

CallgrindParser::~CallgrindParser()
{
}

void CallgrindParser::parseNewCallgrindItem(const QString& line, bool programTotal)
{
    static const QRegularExpression numCallsExpression("\\((\\d+)x\\)");
    static const QRegularExpression objExpression("\\[(.+)\\]");

    QStringList lineItems = line.split(QChar(' '), QString::SkipEmptyParts);
    for (int i = 0; i < m_events.size(); ++i) {
        lineItems[i].remove(",");
    }

    if (programTotal) {
        m_totalCountItem = getOrCreateNewItem(QStringLiteral(" :Program_Total_Count"));
        for (int i = 0; i < m_events.size(); ++i) {
            m_totalCountItem->setValue(m_events[i], lineItems[i]);
        }
        m_model->newItem(m_totalCountItem);

        return;
    }

    const char lineType = lineItems.takeAt(m_events.size()).at(0).toLatin1();

    QString fullDescName;
    while (lineItems.size() > m_events.size()) {
        fullDescName += lineItems.takeAt(m_events.size()) + ' ';
    }

    int numCalls = 0;
    auto match = numCallsExpression.match(fullDescName);
    if (match.hasMatch()) {
        numCalls = match.captured(1).toInt();
        fullDescName.remove(match.captured(0));
    }

    match = objExpression.match(fullDescName);
    if (match.hasMatch()) {
        fullDescName.remove(match.captured(0));
    }

    auto csItem = getOrCreateNewItem(fullDescName.trimmed());
    for (int j = 0; j < lineItems.size(); ++j) {
        csItem->setValue(m_events[j], lineItems[j]);
    }

    // the function itself
    if (lineType == '*') {
        if (m_caller.size() > 0) {
            foreach (CallgrindCallstackItem* caller, m_caller) {
                csItem->addParent(caller);
                caller->addChild(csItem);
            }
            m_caller.clear();
        }

        csItem->setNumCalls(m_numCalls);

        m_numCalls = 0;
        m_allFunctions.push_back(csItem);
        m_lastCall = csItem;
        m_model->newItem(csItem);
    }

    // a caller
    else if (lineType == '<') {
        m_caller.append(csItem);

        // Only add the number of calls here,
        // since the total number of calls, is the sum of calls from callers
        m_numCalls += numCalls;
    }

    // a called function
    else if (lineType == '>') {
        m_lastCall->addChild(csItem);
        csItem->addParent(m_lastCall);
    }

    else {
        qCWarning(KDEV_VALGRIND) << "unknown line type:" << lineType;
    }
}

CallgrindCallstackItem* CallgrindParser::getOrCreateNewItem(const QString& fullDescName)
{
    CallgrindCallstackFunction* csFct = nullptr;

    foreach (const CallgrindCallstackItem* item, m_allFunctions) {
        if (item->csFunction()->fullDescName() == fullDescName) {
            csFct = item->csFunction();
            break;
        }
    }

    if (!csFct) {
        csFct = new CallgrindCallstackFunction();
        csFct->setFullDescName(fullDescName);
        csFct->setTotalCountItem(m_totalCountItem);
    }

    return new CallgrindCallstackItem(csFct);
}

enum CallgrindParserState
{
    ParseRootModel,
    ParseProgramTotalHeader,
    ParseProgramTotal,
    ParseProgramHeader,
    ParseProgram
};

void CallgrindParser::parse(QByteArray& baData, CallgrindModel* model)
{
    Q_ASSERT(model);
    m_model = model;

    CallgrindParserState parserState = ParseRootModel;
    QString eventsString;
    QString line;

    QBuffer data(&baData);
    data.open(QIODevice::ReadOnly);

    while (!data.atEnd()) {
        line = data.readLine().simplified();

        if (line.startsWith("--") && line.contains("annotated source:")) {
                break;
        }

        if (parserState == ParseRootModel) {
            if (line.startsWith("Events shown:")) {
                // 13 is 'Events shown:' size;
                eventsString = line.mid(13).simplified();
                m_events = eventsString.split(QChar(' '), QString::SkipEmptyParts);
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
                parseNewCallgrindItem(line, true);
                parserState = ParseProgramHeader;
            }
            else {
                parseNewCallgrindItem(line, false);
            }
        }
    }

    // Null item is send when the parsing has been done
    m_model->newItem(nullptr);

    m_model = nullptr;
}

}
