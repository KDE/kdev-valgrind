/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
 * Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#include "model.h"

#include "debug.h"
#include <kmessagebox.h>
#include <QFontDatabase>

#include "modelitem.h"

#include <klocalizedstring.h>

namespace valgrind
{

MassifModel::MassifModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    m_rootItem = new MassifItem();
    m_rootItem->incomingData("snapshot", "");
    m_rootItem->incomingData("time", "");
    m_rootItem->incomingData("mem_heap_B", "");
    m_rootItem->incomingData("mem_heap_extra_B", "");
    m_rootItem->incomingData("mem_stacks_B", "");
}

MassifModel::~MassifModel()
{
    delete m_rootItem;
}

void MassifModel::newItem(MassifItem* i)
{
    if (!i) {
        return;
    }

    i->setParent(m_rootItem);
    m_rootItem->appendChild(i);
}

void MassifModel::reset()
{
}

QVariant MassifModel::getColumnAtSnapshot(int snap, MassifItem::Columns col)
{
    return m_rootItem->child(snap)->data(col);
}

QModelIndex MassifModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    MassifItem* parentItem;
    if (!parent.isValid()) {
        parentItem = m_rootItem;
    } else {
        parentItem = static_cast<MassifItem*>(parent.internalPointer());
    }

    MassifItem* childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }

    return QModelIndex();
}

QModelIndex MassifModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    MassifItem* childItem = static_cast<MassifItem*>(index.internalPointer());
    MassifItem* parentItem = childItem->parent();
    if (parentItem == m_rootItem) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int MassifModel::rowCount(const QModelIndex &parent) const
{
    MassifItem* parentItem;
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        parentItem = m_rootItem;
    } else {
        parentItem = static_cast<MassifItem*>(parent.internalPointer());
    }

    return parentItem->childCount();
}

int MassifModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return static_cast<MassifItem*>(parent.internalPointer())->columnCount();
    }

    return m_rootItem->columnCount();
}

QVariant MassifModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole: {
        MassifItem *item = static_cast<MassifItem*>(index.internalPointer());
        return item->data(index.column());
    }
    break;

    case Qt::FontRole: {
        QFont f = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
        if ((static_cast<MassifItem*>(index.internalPointer()))->parent() == m_rootItem) {
            f.setBold(true);
        }
        return f;
    }
    break;
    }

    return QVariant();
}

QVariant MassifModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)
    if (role == Qt::DisplayRole) {
        switch (section) {
        case MassifItem::Snapshot:
            return i18n("snapshot");
        case MassifItem::Time:
            return i18n("time");
        case MassifItem::MemHeapB:
            return i18n("mem_heap_B");
        case MassifItem::MemHeapExtraB:
            return i18n("mem_heap_extra_B");
        case MassifItem::MemStacksB:
            return i18n("mem_stacks_B");
        }
    }
    return QVariant();
}

}
