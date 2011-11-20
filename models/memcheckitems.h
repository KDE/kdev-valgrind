/* This file is part of KDevelop
 * Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
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

#ifndef _VALGRINDITEM_H_
#define _VALGRINDITEM_H_

#include <QString>

#include "imodel.h"

namespace valgrind
{
class MemcheckItem : public ModelItem
{

public:

    virtual ~MemcheckItem() {}

    virtual MemcheckItem* parent() const {
        return 0L;
    }

    /*
     * Called when data related to the error has been parsed
     */
    virtual void incomingData(QString name, QString value) = 0;
};
}
#endif /* _VALGRINDITEM_H_ */
