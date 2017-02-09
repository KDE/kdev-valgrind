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

#include "snapshot.h"

#include "debug.h"

namespace valgrind
{

namespace Massif
{

void Snapshot::setValue(const QString& columnName, const QString& value)
{
    int column = -1;
    if (columnName == QStringLiteral("snapshot")) {
        column = SnapshotId;
    }

    else if (columnName == QStringLiteral("time")) {
        column = Time;
    }

    else if (columnName == QStringLiteral("mem_heap_B")) {
        column = Heap;
    }

    else if (columnName == QStringLiteral("mem_heap_extra_B")) {
        column = HeapExtra;
    }

    else if (columnName == QStringLiteral("mem_stacks_B")) {
        column = Stack;
    }

    if (column < 0) {
        qCDebug(KDEV_VALGRIND) << "Massif::Snapshot::setValue(); unknown column name:" << columnName;
        return;
    }

    values[column] = value;
}

}

}
