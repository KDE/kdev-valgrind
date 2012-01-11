/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
 * Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>

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

#include "cachegrindmodel.h"

#include <QApplication>

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kglobalsettings.h>

#include "cachegrinditem.h"

#include "modelwrapper.h"

namespace valgrind
{

CachegrindModel::CachegrindModel(QObject * parent)
{
    Q_UNUSED(parent);
    m_rootItem = 0;
}

CachegrindModel::~CachegrindModel()
{
    if (m_rootItem != 0)
        delete m_rootItem;
}

void CachegrindModel::newItem(valgrind::ModelItem *item)
{
    if (!item)
    {
        emit static_cast<ModelEvents *>(m_modelWrapper)->modelChanged();
        return;
    }
    if (m_rootItem == 0)
    {
        m_rootItem = dynamic_cast<CachegrindItem *>(item);
    }
    else
    {
        CachegrindItem *ci = dynamic_cast<CachegrindItem *>(item);
        Q_ASSERT(ci);
        ci->setParent(m_rootItem);
        m_rootItem->appendChild(ci);
    }
}

void CachegrindModel::reset()
{
}

QModelIndex CachegrindModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    CachegrindItem *parentItem;
    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<CachegrindItem *>(parent.internalPointer());
    if (parentItem == 0)
        return QModelIndex();
    CachegrindItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex CachegrindModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();
    CachegrindItem *childItem = static_cast<CachegrindItem*>(index.internalPointer());
    CachegrindItem *parentItem = childItem->parent();
    if (parentItem == m_rootItem)
        return QModelIndex();
    return createIndex(parentItem->row(), 0, parentItem);
}

int CachegrindModel::rowCount(const QModelIndex &parent) const
{
    CachegrindItem *parentItem;
    if (parent.column() > 0)
        return 0;
    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<CachegrindItem*>(parent.internalPointer());
    if (parentItem != 0)
        return parentItem->childCount();
    return 0;
}

int CachegrindModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<CachegrindItem*>(parent.internalPointer())->columnCount();
    else if (m_rootItem != 0)
        return m_rootItem->columnCount();
    return 0;
}

QVariant CachegrindModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();
    switch (role)
    {
    case Qt::DisplayRole:
    {
        CachegrindItem *item = static_cast<CachegrindItem *>(index.internalPointer());
        return item->data(index.column());
    }
    break;
    case Qt::FontRole:
    {
        QFont f = KGlobalSettings::generalFont();
        if ((static_cast<CachegrindItem*>(index.internalPointer()))->childCount() > 0)
            f.setBold(true);
        return f;
    }
    break;
    }
    return QVariant();
}

QVariant CachegrindModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)
    if (role == Qt::DisplayRole)
    {
        switch (section)
        {
        case CachegrindItem::CallName:
            return i18n("Call name");
        case CachegrindItem::FileName:
            return i18n("File name");
        case CachegrindItem::InstructionRead:
            return i18n("Ir (I cache read)");
        case CachegrindItem::InstructionL1ReadMiss:
            return i18n("I1mr (L1 cache read miss)");
        case CachegrindItem::InstructionLLReadMiss:
            return i18n("ILmr (LL cache read miss)");
        case CachegrindItem::DataCacheRead:
            return i18n("Dr (D cache read)");
        case CachegrindItem::DataCacheD1ReadMiss:
            return i18n("D1mr (D1 cache read miss)");
        case CachegrindItem::DataCacheLLReadMiss:
            return i18n("DLmr (DL cache read miss)");
        case CachegrindItem::DataCacheWrite:
            return i18n("Dw (D cache read)");
        case CachegrindItem::DataCacheD1WriteMiss:
            return i18n("D1mw (D1 cache write miss)");
        case CachegrindItem::DataCacheLLWriteMiss:
            return i18n("DLmw (DL cache write miss)");
        case CachegrindItem::ConditionnalBranchExecute:
            return i18n("Bc (Conditional branches executed)");
        case CachegrindItem::ConditionnalBranchMisprediced:
            return i18n("Bcm (conditional branches mispredicted)");
        case CachegrindItem::IndirectBranchExecuted:
            return i18n("Bi (Indirect branches executed)");
        case CachegrindItem::IndirectBranchMispredicted:
            return i18n("Bim (indirect branches mispredicted)");
        case CachegrindItem::Unknow:
            return i18n("???");
        }
    }
    return QVariant();
}

CachegrindItem* CachegrindModel::itemForIndex(const QModelIndex& index) const
{
    if (index.internalPointer())
        return static_cast<CachegrindItem*>(index.internalPointer());
    return 0L;
}
}

#include "cachegrindmodel.moc"
