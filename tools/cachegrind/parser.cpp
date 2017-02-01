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
#include "modelitem.h"

#include <QBuffer>

namespace valgrind
{

CachegrindParser::CachegrindParser(QObject* parent)
    : QObject(parent)
    , m_model(nullptr)
    , m_lastCall(nullptr)
    , totalCountItem(nullptr)
{
}

CachegrindParser::~CachegrindParser()
{
}

bool CachegrindParser::parseRootModel(const QString& buffer)
{
    m_headersList = buffer.split(QChar(' '), QString::SkipEmptyParts);
    CachegrindItem* rootItem = new CachegrindItem;
    m_programTotalStr = QStringLiteral("");

    for (int i = 0; i < m_headersList.size(); ++i) {
        CachegrindItem::Columns key = CachegrindItem::dataKeyFromName(m_headersList[i]);
        if (key != CachegrindItem::Unknow) {
            m_programTotalStr += " " + m_headersList[i];
            rootItem->incomingData(m_headersList[i], "");
        }

        else {
            qCDebug(KDEV_VALGRIND) << "Error : " << m_headersList[i] << " unknow header";
            return false;
        }
    }

    rootItem->incomingData(CachegrindItem::dataKey(CachegrindItem::CallName), QStringLiteral(""));
    rootItem->incomingData(CachegrindItem::dataKey(CachegrindItem::FileName), QStringLiteral(""));
    m_programTotalStr = m_programTotalStr.trimmed();
    m_model->newItem(rootItem);

    return true;
}

void CachegrindParser::parseNewCachegrindItem(const QString& buffer, bool totalProgram)
{
    CachegrindItem* item(new CachegrindItem);
    int iBegin;
    int iEnd;
    QString appendStr;

    iBegin = iEnd = 0;
    for (int i = 0; i < m_headersList.size(); ++i) {
        if ((iEnd = buffer.indexOf(QChar(' '), iBegin)) == -1)
            break;

        item->incomingData(CachegrindItem::dataKey(CachegrindItem::dataKeyFromName(m_headersList[i])),
                           buffer.mid(iBegin, iEnd - iBegin).replace(',', ""));
        iBegin = iEnd + 1;
    }

    if (!totalProgram) {
        if (buffer.at(iBegin) == QChar('*')) { // Call graph
            if (m_caller.size() > 0) {
                for (int j = 0; j < m_caller.size(); ++j) {
                    item->appendChild(m_caller[j]);
                    m_caller[j]->setParent(item);
                }
                m_caller.clear();
            }

            m_lastCall = item;
            iBegin = buffer.indexOf(QChar(' '), iBegin) + 1;
            totalCountItem->appendChild(item);
            item->setParent(totalCountItem);
        }

        else if (buffer.at(iBegin) == QChar('<')) {
            appendStr = QStringLiteral("< ");
            m_caller.append(item);
            iBegin = buffer.indexOf(QChar(' '), iBegin) + 1;
        }

        else if (buffer.at(iBegin) == QChar('>')) {
            appendStr = QStringLiteral("> ");
            m_lastCall->appendChild(item);
            item->setParent(m_lastCall);
            iBegin = buffer.indexOf(QChar(' '), iBegin) + 1;
        }

        else {
            totalCountItem->appendChild(item);
            item->setParent(totalCountItem);
        }

        if ((iEnd = buffer.indexOf(QChar(':'), iBegin)) == -1)
            return;

        // file name
        item->incomingData(CachegrindItem::dataKey(CachegrindItem::FileName),
                           buffer.mid(iBegin, iEnd - iBegin));

        // call name
        iBegin = iEnd + 1;
        iEnd = buffer.length();
        item->incomingData(CachegrindItem::dataKey(CachegrindItem::CallName),
                           appendStr + buffer.mid(iBegin, iEnd - iBegin));
    }

    else {
        item->incomingData(CachegrindItem::dataKey(CachegrindItem::CallName),
                           QStringLiteral("Program Total Count"));
        item->incomingData(CachegrindItem::dataKey(CachegrindItem::FileName),
                           QStringLiteral(""));
        totalCountItem = item;
        m_model->newItem(item);
    }
}

enum CachegrindParserState
{
    ParseRootModel,
    ParseProgramTotalHeader,
    ParseProgramTotal,
    ParseProgramHeader,
    ParseProgram
};

void CachegrindParser::parse(QByteArray& baData, CachegrindModel* model)
{
    Q_ASSERT(model);
    m_model = model;

    CachegrindParserState parserState(ParseRootModel);
    QBuffer data(&baData);
    QString buffer;

    data.open(QIODevice::ReadOnly);
    while (!data.atEnd())
    {
        // remove useless characters
        buffer = data.readLine().simplified();

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
                    parseNewCachegrindItem(buffer, true);
                    parserState = ParseProgramHeader;
                }
                else
                    parseNewCachegrindItem(buffer, false);
            }
        }
    }

    m_model->newItem(nullptr);
    m_model = nullptr;
}

}
