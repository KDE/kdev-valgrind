/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 201-2017 Anton Anikin <anton.anikin@htower.ru>

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

#pragma once

#include <QByteArray>
#include <QList>

namespace valgrind
{

class CallgrindCallstackItem;
class CallgrindModel;

class CallgrindParser
{
public:
    CallgrindParser();
    ~CallgrindParser();

    void parse(QByteArray& data, CallgrindModel* model);

private:
    void parseNewCallgrindItem(const QString& line, bool programTotal = false);

    CallgrindCallstackItem* getOrCreateNewItem(const QString& fullDescName);

    CallgrindModel* m_model;

    QList<CallgrindCallstackItem*> m_caller;
    CallgrindCallstackItem* m_lastCall;

    QStringList m_events;

    // Temporary storage for the number of calls
    // of a function that's being parsed
    int m_numCalls;

    CallgrindCallstackItem* m_totalCountItem;

    QList<CallgrindCallstackItem*> m_allFunctions;
};

}
