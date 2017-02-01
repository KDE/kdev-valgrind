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

#include "modelitem.h"
#include "debug.h"

namespace valgrind
{

MassifItem::MassifItem(bool isChild)
    : m_parent(nullptr)
    , m_isChild(isChild)
    , m_line(0)
{
}

MassifItem::~MassifItem()
{
    qDeleteAll(m_childs);
}

void MassifItem::incomingData(const QString& name, const QString& value, const QString& dir)
{
    m_values[name] = value;
    if (name == QStringLiteral("child")) {
        QStringList lst = value.mid(value.lastIndexOf('(') + 1).remove(')').split(':');
        if (lst.size() != 2) {
            return;
        }

        m_dir = dir;
        m_file = lst[0];
        m_line = lst[1].toInt();
    }
}

void MassifItem::addChild(MassifItem* item)
{
    m_childs.append(item);
    item->m_parent = this;
}

MassifItem* MassifItem::child(int row)
{
    return m_childs.value(row);
}

int MassifItem::childCount() const
{
    return m_childs.count();
}

int MassifItem::columnCount() const
{
    return m_values.size();
}

QVariant MassifItem::data(int column) const
{
    if (m_isChild && column == MemStacksB) {
        return m_values[QStringLiteral("child")];
    }

    switch (column)
    {

    case Snapshot:
        return m_values[QStringLiteral("snapshot")];

    case Time:
        return m_values[QStringLiteral("time")];

    case MemHeapB:
        return m_values[QStringLiteral("mem_heap_B")];

    case MemHeapExtraB:
        return m_values[QStringLiteral("mem_heap_extra_B")];

    case MemStacksB:
        return m_values[QStringLiteral("mem_stacks_B")];

    }

    return QVariant();
}

MassifItem* MassifItem::parent()
{
    return m_parent;
}

int MassifItem::row() const
{
    if (m_parent) {
        return m_parent->m_childs.indexOf(const_cast<MassifItem*>(this));
    }

    return 0;
}

QUrl MassifItem::url() const
{
    if (m_dir.isEmpty() && m_file.isEmpty()) {
        return QUrl();
    }

    QUrl base = QUrl::fromLocalFile(m_dir);
    base.setPath(base.path() + '/');
    QUrl url = base.resolved(QUrl(m_file));

    return url;
}

int MassifItem::line() const
{
    return m_line;
}

}
