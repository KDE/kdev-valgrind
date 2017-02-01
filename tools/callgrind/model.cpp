/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
 * Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
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
#include <klocalizedstring.h>

#include "modelitem.h"

namespace valgrind
{

///////////////////////////////////////////////////////////////////////////////
//////////////////////////CallgrindModel implementation////////////////////////
///////////////////////////////////////////////////////////////////////////////

CallgrindModel::CallgrindModel(QObject* parent)
    : QObject(parent)
{
    m_callgrindFunctionModel = new CallgrindFunctionsListTModel(this);
    m_totalCountItem = nullptr;
}

CallgrindModel::~CallgrindModel()
{
    for (int i = 0; i < m_callgrindCsItems.size(); ++i)
    {
        if (m_callgrindCsItems[i] != nullptr) {
            delete m_callgrindCsItems[i];
        }
    }

    if (m_totalCountItem) {
        delete m_totalCountItem;
    }

    delete m_callgrindFunctionModel;
}

void CallgrindModel::newItem(CallgrindCsItem* item)
{
    //Null item is send when the parsing has been done
    if (!item)
    {
        //set the function list model
        m_callgrindFunctionModel->setItemList(m_callgrindCsItems);
        //emit static_cast<ModelEvents *>(m_modelWrapper)->modelChanged();
        return;
    }
    CallgrindCsItem *csItem = dynamic_cast<CallgrindCsItem *>(item);
    Q_ASSERT(csItem);
    if ( m_totalCountItem == nullptr ) {
        m_totalCountItem = csItem;
    } else {
        m_callgrindCsItems.append(csItem);
    }
}

void CallgrindModel::reset()
{
}

const QList<CallgrindCallstackItem*>& CallgrindModel::getAllCsItem() const
{
    return this->m_callgrindCsItems;
}

QVariant CallgrindModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)
    if (role == Qt::DisplayRole)
    {
        switch (section)
        {
        case iCachegrindItem::CallName:
            return i18n("Call name");
        case iCachegrindItem::FileName:
            return i18n("File name");
        case iCachegrindItem::InstructionRead:
            return i18n("Ir (I cache read)");
        case iCachegrindItem::InstructionL1ReadMiss:
            return i18n("I1mr (L1 cache read miss)");
        case iCachegrindItem::InstructionLLReadMiss:
            return i18n("ILmr (LL cache read miss)");
        case iCachegrindItem::DataCacheRead:
            return i18n("Dr (D cache read)");
        case iCachegrindItem::DataCacheD1ReadMiss:
            return i18n("D1mr (D1 cache read miss)");
        case iCachegrindItem::DataCacheLLReadMiss:
            return i18n("DLmr (DL cache read miss)");
        case iCachegrindItem::DataCacheWrite:
            return i18n("Dw (D cache write)");
        case iCachegrindItem::DataCacheD1WriteMiss:
            return i18n("D1mw (D1 cache write miss)");
        case iCachegrindItem::DataCacheLLWriteMiss:
            return i18n("DLmw (DL cache write miss)");
        case iCachegrindItem::ConditionnalBranchExecute:
            return i18n("Bc (Conditional branches executed)");
        case iCachegrindItem::ConditionnalBranchMisprediced:
            return i18n("Bcm (conditional branches mispredicted)");
        case iCachegrindItem::IndirectBranchExecuted:
            return i18n("Bi (Indirect branches executed)");
        case iCachegrindItem::IndirectBranchMispredicted:
            return i18n("Bim (indirect branches mispredicted)");
        case iCachegrindItem::NumberOfCalls:
            return i18n("Number of calls");
        case iCachegrindItem::Unknow:
            return i18n("???");
        }
    }
    return QVariant();
}

QAbstractItemModel* CallgrindModel::abstractItemModel(int n)
{
    //this model is able to handle many model, it isn't used yet but it can be
    switch (n)
    {
    case E_FCT_LIST:
        return (m_callgrindFunctionModel);
    default:
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////
////////////////CallgrindFunctionsListTModel implementation////////////////////
///////////////////////////////////////////////////////////////////////////////

CallgrindFunctionsListTModel::CallgrindFunctionsListTModel(CallgrindModel *mdl)
{
    Q_ASSERT(mdl);
    m_model = mdl;
}

void  CallgrindFunctionsListTModel::setItemList(const QList<CallgrindCallstackItem *> items)
{
    emit layoutAboutToBeChanged();
    for (int i = 0; i < items.size() ; ++i)
    {
        //here function list can be filtraded to display only high percents function
        //if (items.at(i)->getNumericValuePercent(iCachegrindItem::InstructionRead) >= CALLGRIND_MIN_DISPLAY_PERCENT)
        //{
        m_items.push_back(items.at(i));
        //}
    }
    //m_items = items;
    emit layoutChanged();
}

QModelIndex CallgrindFunctionsListTModel::index(int row, int column, const QModelIndex &parent) const
{
    //Q_UNUSED(parent);
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    return createIndex(row, column, m_items[row]);
}

QModelIndex CallgrindFunctionsListTModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int CallgrindFunctionsListTModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_items.count();
    }

    return 0;
}

int CallgrindFunctionsListTModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        // Call name, incl., IR, Number of calls
        return 4;
    }

    return 0;
}

QVariant CallgrindFunctionsListTModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        CallgrindCallstackItem *item = static_cast<CallgrindCallstackItem *>(index.internalPointer());
        return item->data(columnToPosInModelList(index.column()), columnToDisplayMode(index.column()));
    }

    return QVariant();
}

QVariant CallgrindFunctionsListTModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (section == 1) {
            return "Incl.";
        }

        return m_model->headerData(columnToPosInModelList(section), orientation, role);
    }
    return QVariant();

}

CallgrindCallstackItem* CallgrindFunctionsListTModel::itemForIndex(const QModelIndex& index) const
{
    if (index.internalPointer()) {
        return static_cast<CallgrindCallstackItem*>(index.internalPointer());
    }

    return nullptr;
}

void CallgrindFunctionsListTModel::quickSortCSItem( int first, int last,
        QList<CallgrindCallstackItem*>& list,
        iCachegrindItem::Columns col,
        CSItemCompareFct cmp,
        CallgrindCallstackItem::numberDisplayMode dispMode )
{
    if (first < last && first != last)
    {
        int left  = first;
        int right = last;
        int pivot = left++;

        while (left != right)
        {
            if ((this->*cmp)( list[left]->data(col, dispMode), list[pivot]->data(col, dispMode) ))
            {
                ++left;
            }
            else
            {
                while ((left != right) && (this->*cmp)( list[pivot]->data(col, dispMode), list[right]->data(col, dispMode) )) {
                    --right;
                }
                list.swap(left, right);
            }
        }

        --left;
        list.swap(pivot, left);

        quickSortCSItem( first, left, list, col, cmp, dispMode );
        quickSortCSItem( right, last, list, col, cmp, dispMode );
    }
}

void  CallgrindFunctionsListTModel::sort(int col, Qt::SortOrder order)
{
    int size = this->rowCount() - 1;
    iCachegrindItem::Columns cachegrindCol = this->columnToPosInModelList(col);
    CallgrindCallstackItem::numberDisplayMode dispModel = columnToDisplayMode(col);
    CSItemCompareFct compareFct = &CallgrindFunctionsListTModel::lessThan;
    if (order == Qt::AscendingOrder) {
        compareFct = &CallgrindFunctionsListTModel::greatherThan;
    }

    emit layoutAboutToBeChanged();
    quickSortCSItem(0, size, m_items, cachegrindCol, compareFct, dispModel);
    emit layoutChanged();
}

bool CallgrindFunctionsListTModel::lessThan(const QVariant &left, const QVariant &right) const
{
    if (left.type() == QVariant::String)
    {
        QString leftStr = left.toString();
        QString rightStr = right.toString();
        return QString::localeAwareCompare(leftStr, rightStr) <= 0;
    }
    else if (left.type() == QVariant::Int || left.type() == QVariant::LongLong ||
             left.type() == QVariant::UInt || left.type() == QVariant::ULongLong)
    {
        return left.toULongLong() <= right.toULongLong();
    }
    return true;
}

bool CallgrindFunctionsListTModel::greatherThan(const QVariant &left, const QVariant &right) const
{
    if (left.type() == QVariant::String)
    {
        QString leftStr = left.toString();
        QString rightStr = right.toString();
        return QString::localeAwareCompare(leftStr, rightStr) >= 0;
    }
    else if (left.type() == QVariant::Int || left.type() == QVariant::LongLong ||
             left.type() == QVariant::UInt || left.type() == QVariant::ULongLong ||
             left.type() == QVariant::Double)
    {
        return left.toULongLong() >= right.toULongLong();
    }
    //unknow type
    return true;
}

iCachegrindItem::Columns   CallgrindFunctionsListTModel::columnToPosInModelList(int col) const
{
    //qCDebug(KDEV_VALGRIND) << "col " << col;
    switch(col)
    {
    case 0:
        return iCachegrindItem::CallName;
    case 1:
        return iCachegrindItem::InstructionRead;
    case 2:
        return iCachegrindItem::InstructionRead;
    case 3:
        return iCachegrindItem::NumberOfCalls;
    default:
        return iCachegrindItem::Unknow;
    }
}

CallgrindCallstackItem::numberDisplayMode CallgrindFunctionsListTModel::columnToDisplayMode(int col) const
{
    if (col == 1) {
        return CallgrindCallstackItem::E_INCLUDE_PERCENT;
    }

    return CallgrindCallstackItem::E_PERCENT;
}

}
