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

#include "modelitem.h"
#include "debug.h"

namespace valgrind
{

CallgrindParser::CallgrindParser(QObject* parent)
    : Parser(parent)
    , m_lastCall(nullptr)
    , m_numCalls(0)
    , m_totalCountItem(nullptr)
{
}

CallgrindParser::~CallgrindParser()
{
}

bool CallgrindParser::parseRootModel(const QString& buffer)
{
    m_headersList = buffer.split(QChar(' '), QString::SkipEmptyParts);
    m_programTotalStr = QStringLiteral("");

    for (int i = 0; i < m_headersList.size(); ++i) {
        iCachegrindItem::Columns key = iCachegrindItem::dataKeyFromName(m_headersList[i]);
        if (key != iCachegrindItem::Unknow)
            m_programTotalStr += " " + m_headersList[i];
        else {
            qCDebug(KDEV_VALGRIND) << "Error : " << m_headersList[i] << " unknow header";
            return false;
        }
    }

    m_programTotalStr = m_programTotalStr.trimmed();
    return true;
}

void CallgrindParser::parseNewCallgrindItem(const QString& buffer, bool totalProgram)
{
    CallgrindCallstackItem* csItem;
    int iBegin;
    int iEnd;
    QStringList dataList;

    int numCalls = 0;

    // Find if we have the number of calls
    // sample:  /some/path/somefile:somefunction() (666x)
    // where 666 is the number of calls
    int idx = buffer.lastIndexOf('(');
    if (idx != -1) {
        int idx2 = buffer.indexOf(QStringLiteral("x)"), idx); // lastIndexOf somehow returns -1 here
        if (idx2 != -1) {
            // Grab the number of calls
            // Example:
            // (666x) => 666
            QString sNumCalls = buffer.mid(idx + 1, idx2 - idx - 1);
            numCalls = sNumCalls.toInt();
        }
    }

    iBegin = iEnd = 0;
    for (int i = 0; i < m_headersList.size(); ++i) {
        if ((iEnd = buffer.indexOf(QChar(' '), iBegin)) == -1)
            break;

        dataList.append(buffer.mid(iBegin, iEnd - iBegin).replace(',', ""));
        iBegin = iEnd + 1;
    }

    if (!totalProgram) {
        if (buffer.at(iBegin) == QChar('*') ||
            buffer.at(iBegin) == QChar('<') ||
            buffer.at(iBegin) == QChar('>')) {

            char symbol = buffer.at(iBegin).toLatin1();
            iBegin = buffer.indexOf(QChar(' '), iBegin) + 1;

            if ((iEnd = buffer.indexOf(QChar(')'), iBegin)) == -1) {
                if ((iEnd = buffer.indexOf(QChar('['), iBegin)) == -1)
                    iEnd = buffer.length();
            }
            else
                iEnd += 1;

            csItem = getOrCreateNewItem(buffer.mid(iBegin, iEnd - iBegin));

            switch (symbol) {

            // The function itself
            case '*':
                if (m_caller.size() > 0) {
                    for (int j = 0; j < m_caller.size(); ++j) {
                        csItem->addParent(m_caller[j]);
                        m_caller[j]->addChild(csItem);
                    }
                    m_caller.clear();
                }

                csItem->setNumCalls(m_numCalls);

                m_numCalls = 0;
                m_allFunctions.push_back(csItem);
                m_lastCall = csItem;
                emit newItem(csItem);
                break;

            // a caller
            case '<':
                m_caller.append(csItem);

                // Only add the number of calls here,
                // since the total number of calls, is the sum of calls from callers
                m_numCalls += numCalls;
                break;

            // a called function
            case '>':
                m_lastCall->addChild(csItem);
                csItem->addParent(m_lastCall);
                break;
            }

        }

        else {
            if ((iEnd = buffer.indexOf(QChar(')'), iBegin)) == -1)
                if ((iEnd = buffer.indexOf(QChar('['), iBegin)) == -1)
                    iEnd = buffer.length();

            csItem = getOrCreateNewItem(buffer.mid(iBegin, iEnd - iBegin));
            emit newItem(csItem);
        }

        for (int j = 0; j < dataList.size(); ++j) {
            csItem->incommingData(m_headersList[j], dataList[j]);
        }
    }

    else {
        //total program count
        CallgrindCallstackItem* totalCount = getOrCreateNewItem(QStringLiteral(" :Program_Total_Count"));
        for (int j = 0; j < dataList.size(); ++j) {
            totalCount->incommingData(m_headersList[j], dataList[j]);
        }

        m_totalCountItem = totalCount;
        emit newItem(totalCount);
    }
}

CallgrindCallstackItem* CallgrindParser::getOrCreateNewItem(const QString& fullDescName)
{
    CallgrindCallstackFunction* csFct = nullptr;
    //qCDebug(KDEV_VALGRIND) << fullDescName;

    for (int i = 0; i < m_allFunctions.size(); ++i) {
        if (m_allFunctions[i]->getCsFunction()->getFullDescName().compare(fullDescName) == 0) {
            csFct = m_allFunctions[i]->getCsFunction();
            break;
        }
    }

    if (csFct == nullptr) {
        csFct = new CallgrindCallstackFunction();
        csFct->setFullDescName(fullDescName);
        csFct->setTotalCountItem(m_totalCountItem);
    }

    CallgrindCallstackItem* item = new CallgrindCallstackItem(csFct);
    return item;
}

enum CallgrindParserState
{
    ParseRootModel,
    ParseProgramTotalHeader,
    ParseProgramTotal,
    ParseProgramHeader,
    ParseProgram
};

void CallgrindParser::parse()
{
    CallgrindParserState parserState(ParseRootModel);
    QString buffer;

    while (!device()->atEnd()) {
        //remove useless characters
        buffer = device()->readLine().simplified();

        if (parserState != ParseProgramTotal && parserState != ParseProgram) {
            if (parserState == ParseRootModel && buffer.startsWith("Events shown:")) {
                //13 is 'Events shown:' size;
                if (!parseRootModel(buffer.mid(13, buffer.length() - 13))) {
                    qCDebug(KDEV_VALGRIND) << "Input stream is misformated, cannot build the tree";
                    return ;
                }
                parserState = ParseProgramTotalHeader;
            }

            else if (parserState == ParseProgramTotalHeader && buffer == m_programTotalStr)
                parserState = ParseProgramTotal;

            else if (parserState == ParseProgramHeader && buffer.startsWith(m_programTotalStr))
                parserState = ParseProgram;
        }

        else {
            if (buffer.size() > 0 && buffer.at(0).isDigit()) {
                if (parserState == ParseProgramTotal) {
                    parseNewCallgrindItem(buffer, true);
                    parserState = ParseProgramHeader;
                }
                else
                    parseNewCallgrindItem(buffer, false);
            }
        }
    }

    emit newItem(nullptr);
}

}
