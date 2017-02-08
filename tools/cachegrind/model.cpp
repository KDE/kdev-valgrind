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
{
}

CachegrindModel::~CachegrindModel()
{
    qDeleteAll(m_items);
}

void CachegrindModel::addItem(CachegrindItem* item)
{
    Q_ASSERT(item);
    if (item) {
        m_items.append(item);
    }
}

void CachegrindModel::setEventsList(const QStringList& eventsList)
{
    m_eventList = eventsList;
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
    return m_eventList.size() + 2;
}

QVariant CachegrindModel::data(const QModelIndex& index, int role) const
{
    auto item = static_cast<CachegrindItem*>(index.internalPointer());
    if (!item) {
        return QVariant();
    }

    if (role == Qt::TextAlignmentRole &&
        index.column() != 0 &&
        index.column() != (columnCount() - 1)) {

        return rightAlign;
    }

    if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
            return item->callName;
        }

        if (index.column() == (columnCount() - 1)) {
            return item->fileName;
        }

        QString event = m_eventList[index.column() - 1];
        return displayValue(item->eventValues[event].toInt());
    }

    if (role == Qt::FontRole) {
        QFont f = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
        if (item->isProgramTotal) {
            f.setBold(true);
        }
        return f;
    }

    return QVariant();
}

QVariant CachegrindModel::headerData(int section, Qt::Orientation, int role) const
{
    if (section == 0) {
        if (role == Qt::DisplayRole) {
            return i18n("Call name");
        }
    }

    else if (section == (columnCount() - 1)) {
        if (role == Qt::DisplayRole) {
            return i18n("File name");
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
