/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef _CACHEGRINDPARSER_H_
#define _CACHEGRINDPARSER_H_

#include "debug.h"
#include <kmessagebox.h>
#include "imodel.h"
#include "iparser.h"

namespace valgrind
{

class CachegrindItem;

/**
 * A class which parses valgrind's XML output
 * and emits signals when items are parsed
 */
class CachegrindParser : public Parser
{
    Q_OBJECT

public:

    CachegrindParser(QObject *parent = 0);
    virtual ~CachegrindParser();

private:
    /**
     * build the root node of the tree
     * it return false if some error happens
     */
    bool parseRootModel(const QString &buffer);

    void parseNewCachegrindItem(const QString& buffer, bool totalProgram = false);

    QList<CachegrindItem *>  m_caller;
    CachegrindItem           *m_lastCall;
    QString                 m_programTotalStr;
    QStringList             m_headersList;

    CachegrindItem *totalCountItem;

signals:

    /**
     * Emits this signal when a new item is parsed
     */
    void newItem(valgrind::ModelItem*);


public slots:
    void parse();
};
}


#endif /* _CACHEGRINDPARSER_H_ */
