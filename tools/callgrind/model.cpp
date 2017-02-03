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
#include "modelitem.h"

#include "debug.h"

#include <klocalizedstring.h>

namespace valgrind
{

CallgrindModel::CallgrindModel(QObject* parent)
    : QObject(parent)
    , m_totalCountItem(nullptr)
    , m_callgrindFunctionModel(new CallgrindFunctionsListTModel(this))
{
}

CallgrindModel::~CallgrindModel()
{
    qDeleteAll(m_callgrindCsItems);

    delete m_totalCountItem;
    delete m_callgrindFunctionModel;
}

void CallgrindModel::newItem(CallgrindCallstackItem* item)
{
    // Null item is send when the parsing has been done
    if (!item) {
        // set the function list model
        m_callgrindFunctionModel->setItemList(m_callgrindCsItems);
        return;
    }

    if (!m_totalCountItem) {
        m_totalCountItem = item;
    } else {
        m_callgrindCsItems.append(item);
    }
}

QVariant CallgrindModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)

    if (role == Qt::DisplayRole) {
        switch (section) {

        case CallgrindItem::CallName:
            return i18n("Call name");

        case CallgrindItem::FileName:
            return i18n("File name");

        case CallgrindItem::InstructionRead:
            return i18n("Ir (I cache read)");

        case CallgrindItem::InstructionL1ReadMiss:
            return i18n("I1mr (L1 cache read miss)");

        case CallgrindItem::InstructionLLReadMiss:
            return i18n("ILmr (LL cache read miss)");

        case CallgrindItem::DataCacheRead:
            return i18n("Dr (D cache read)");

        case CallgrindItem::DataCacheD1ReadMiss:
            return i18n("D1mr (D1 cache read miss)");

        case CallgrindItem::DataCacheLLReadMiss:
            return i18n("DLmr (DL cache read miss)");

        case CallgrindItem::DataCacheWrite:
            return i18n("Dw (D cache write)");

        case CallgrindItem::DataCacheD1WriteMiss:
            return i18n("D1mw (D1 cache write miss)");

        case CallgrindItem::DataCacheLLWriteMiss:
            return i18n("DLmw (DL cache write miss)");

        case CallgrindItem::ConditionnalBranchExecute:
            return i18n("Bc (Conditional branches executed)");

        case CallgrindItem::ConditionnalBranchMisprediced:
            return i18n("Bcm (conditional branches mispredicted)");

        case CallgrindItem::IndirectBranchExecuted:
            return i18n("Bi (Indirect branches executed)");

        case CallgrindItem::IndirectBranchMispredicted:
            return i18n("Bim (indirect branches mispredicted)");

        case CallgrindItem::NumberOfCalls:
            return i18n("Number of calls");

        case CallgrindItem::Unknow:
            return i18n("???");

        }
    }

    return QVariant();
}

QAbstractItemModel* CallgrindModel::abstractItemModel(int n)
{
    // this model is able to handle many model, it isn't used yet but it can be
    if (n == E_FCT_LIST) {
        return (m_callgrindFunctionModel);
    }

    return nullptr;
}

CallgrindFunctionsListTModel::CallgrindFunctionsListTModel(CallgrindModel* model)
{
    Q_ASSERT(model);
    m_model = model;
}

void CallgrindFunctionsListTModel::setItemList(const QList<CallgrindCallstackItem*> items)
{
    emit layoutAboutToBeChanged();

    for (int i = 0; i < items.size() ; ++i) {
        // here function list can be filtraded to display only high percents function
        // if (items.at(i)->getNumericValuePercent(CachegrindItem::InstructionRead) >= CALLGRIND_MIN_DISPLAY_PERCENT)
        // {
        m_items.push_back(items.at(i));
        // }
    }

    // m_items = items;

    emit layoutChanged();
}

QModelIndex CallgrindFunctionsListTModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    return createIndex(row, column, m_items[row]);
}

QModelIndex CallgrindFunctionsListTModel::parent(const QModelIndex&) const
{
    return QModelIndex();
}

int CallgrindFunctionsListTModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return m_items.count();
    }

    return 0;
}

int CallgrindFunctionsListTModel::columnCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        // Call name, incl., IR, Number of calls
        return 4;
    }

    return 0;
}

QVariant CallgrindFunctionsListTModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        auto item = static_cast<CallgrindCallstackItem*>(index.internalPointer());
        return item->data(columnToPosInModelList(index.column()), columnToDisplayMode(index.column()));
    }

    return QVariant();
}

QVariant CallgrindFunctionsListTModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (section == 1) {
            return "Incl.";
        }

        return m_model->headerData(columnToPosInModelList(section), orientation, role);
    }

    return QVariant();

}

// FIXME remove and replace with standard sort technique
void CallgrindFunctionsListTModel::quickSortCSItem(
    int first,
    int last,
    QList<CallgrindCallstackItem*>& list,
    CallgrindItem::Columns col,
    CSItemCompareFct cmp,
    CallgrindCallstackItem::numberDisplayMode dispMode)
{
    if (first < last && first != last) {
        int left  = first;
        int right = last;
        int pivot = left++;

        while (left != right) {
            if ((this->*cmp)( list[left]->data(col, dispMode), list[pivot]->data(col, dispMode) )) {
                ++left;
            } else {
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
    emit layoutAboutToBeChanged();

    CSItemCompareFct compareFct = &CallgrindFunctionsListTModel::lessThan;
    if (order == Qt::AscendingOrder) {
        compareFct = &CallgrindFunctionsListTModel::greatherThan;
    }

    quickSortCSItem(0,
                    (this->rowCount() - 1),
                    m_items,
                    columnToPosInModelList(col),
                    compareFct,
                    columnToDisplayMode(col));

    emit layoutChanged();
}

bool CallgrindFunctionsListTModel::lessThan(const QVariant& left, const QVariant& right) const
{
    if (left.type() == QVariant::String) {
        QString leftStr = left.toString();
        QString rightStr = right.toString();
        return QString::localeAwareCompare(leftStr, rightStr) <= 0;
    }

    else if (left.type() == QVariant::Int  || left.type() == QVariant::LongLong ||
             left.type() == QVariant::UInt || left.type() == QVariant::ULongLong) {
        return left.toULongLong() <= right.toULongLong();
    }

    return true;
}

bool CallgrindFunctionsListTModel::greatherThan(const QVariant& left, const QVariant& right) const
{
    if (left.type() == QVariant::String) {
        QString leftStr = left.toString();
        QString rightStr = right.toString();
        return QString::localeAwareCompare(leftStr, rightStr) >= 0;
    }

    else if (left.type() == QVariant::Int  || left.type() == QVariant::LongLong ||
             left.type() == QVariant::UInt || left.type() == QVariant::ULongLong ||
             left.type() == QVariant::Double) {
        return left.toULongLong() >= right.toULongLong();
    }

    return true;
}

CallgrindItem::Columns CallgrindFunctionsListTModel::columnToPosInModelList(int col) const
{
    if (col == 0) {
        return CallgrindItem::CallName;
    }

    if (col == 1) {
        return CallgrindItem::InstructionRead;
    }

    if (col == 2) {
        return CallgrindItem::InstructionRead;
    }

    if (col == 3) {
        return CallgrindItem::NumberOfCalls;
    }

    return CallgrindItem::Unknow;
}

CallgrindCallstackItem::numberDisplayMode CallgrindFunctionsListTModel::columnToDisplayMode(int col) const
{
    if (col == 1) {
        return CallgrindCallstackItem::E_INCLUDE_PERCENT;
    }

    return CallgrindCallstackItem::E_PERCENT;
}

}
