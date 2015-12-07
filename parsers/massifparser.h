/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>

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

#ifndef _MASSIFPARSER_H_
#define _MASSIFPARSER_H_

#include "debug.h"
#include <kmessagebox.h>
#include "imodel.h"
#include "iparser.h"
#include "massifitem.h"

namespace valgrind
{

/**
 * A class which parses valgrind's XML output
 * and emits signals when items are parsed
 */
class MassifParser : public Parser
{
    Q_OBJECT

public:

    MassifParser(QObject *parent = 0);
    virtual ~MassifParser();

signals:

    /**
     * Emits this signal when a new item is parsed
     */
    void newItem(valgrind::ModelItem*);

    void reset();

public slots:
    void parse();


private:
    QString m_workingDir;
    QString m_buffer;
    QStringList m_lst;
    MassifItem *m_item;
};
}


#endif /* _MASSIFPARSER_H_ */
