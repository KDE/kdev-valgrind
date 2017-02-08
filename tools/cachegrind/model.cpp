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
#include "generic/events.h"
#include "generic/model.h"

#include <klocalizedstring.h>
#include <kmessagebox.h>

#include <QFontDatabase>

namespace valgrind
{

CachegrindModel::CachegrindModel(QObject* parent)
    : QAbstractTableModel(parent)
    , m_totalItem(nullptr)
    , m_percentageValues(false)
{
}

CachegrindModel::~CachegrindModel()
{
    qDeleteAll(m_items);
}

void CachegrindModel::addItem(CachegrindItem* newItem, bool isTotal)
{
    Q_ASSERT(newItem);
    if (isTotal) {
        m_totalItem = newItem;
    }

    bool exists = false;
    foreach (auto item, m_items) {
        if (item->functionName == newItem->functionName) {
            exists = true;

            item->fileNames += newItem->fileNames;
            item->fileNames.removeDuplicates();
            item->fileNames.sort();

            for (int i = 0; i < newItem->eventValues.size(); ++i) {
                item->eventValues[i] += newItem->eventValues[i];
            }

            delete newItem;
        }
    }

    if (!exists) {
        m_items.append(newItem);
    }
}

void CachegrindModel::setEventsList(const QStringList& eventsList)
{
    m_eventList = eventsList;
}

void CachegrindModel::setPercentageValues(bool value)
{
    m_percentageValues = value;
    emitDataChanged(this);
}

QModelIndex CachegrindModel::index(int row, int column, const QModelIndex&) const
{
    if (row >= 0 && row < rowCount() && column >= 0 && column < columnCount()) {
        return createIndex(row, column, m_items.at(row));
    }

    return QModelIndex();
}

int CachegrindModel::rowCount(const QModelIndex&) const
{
    return m_items.size();
}

int CachegrindModel::columnCount(const QModelIndex&) const
{
    return m_eventList.size() + 1;
}

QVariant CachegrindModel::data(const QModelIndex& index, int role) const
{
    Q_ASSERT(m_totalItem);

    if (!index.isValid()) {
        return QVariant();
    }

    auto item = static_cast<CachegrindItem*>(index.internalPointer());
    int column = index.column();

    if (role == Qt::TextAlignmentRole && column > 0) {
        return rightAlign;
    }

    if (role == Qt::FontRole && item == m_totalItem) {
        QFont f = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
        f.setBold(true);
        return f;
    }

    if (column == 0) {
        if (role == Qt::DisplayRole || role == SortRole) {
            return item->functionName;
        }
    }

    else {
        if (role == Qt::DisplayRole) {
            int intValue = item->eventValues.at(column - 1);

            if (!m_percentageValues) {
                return displayValue(intValue);
            }

            return displayValue(intValue * 100.0 / m_totalItem->eventValues.at(column - 1));
        }

        if (role == SortRole) {
            return item->eventValues.at(column - 1);
        }
    }

    return QVariant();
}

QVariant CachegrindModel::headerData(int section, Qt::Orientation, int role) const
{
    if (section == 0) {
        if (role == Qt::DisplayRole) {
            return i18n("Function");
        }
    }

    else {
        if (role == Qt::DisplayRole) {
            return m_eventList[section - 1];
        }

        if (role == Qt::ToolTipRole) {
            return eventFullName(m_eventList[section - 1]);
        }
    }

    return QVariant();
}

}
