/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
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

#pragma once

#include <QObject>

namespace valgrind
{

class CallgrindItem;
class CallgrindCallstackItem;
class ModelItem;

/**
 * A class which parses callgrind's output
 * and emits signals when items are parsed
 */
class CallgrindParser : public QObject
{
    Q_OBJECT

public:
    explicit CallgrindParser(QObject* parent = nullptr);
    ~CallgrindParser() override;

    void parse(QByteArray& data);

signals:
    void newItem(ModelItem*);

private:
    /**
     * build the root node of the tree
     * it return false if some error happens
     */
    bool parseRootModel(const QString& buffer);

    void parseNewCallgrindItem(const QString& buffer, bool totalProgram = false);

    CallgrindCallstackItem* getOrCreateNewItem(const QString& fullDescName);

    QList<CallgrindCallstackItem*> m_caller;
    CallgrindCallstackItem* m_lastCall;

    QString m_programTotalStr;
    QStringList m_headersList;

    // Temporary storage for the number of calls
    // of a function that's being parsed
    int m_numCalls;

    CallgrindCallstackItem* m_totalCountItem;

    QList<CallgrindCallstackItem*> m_allFunctions;
};

}
