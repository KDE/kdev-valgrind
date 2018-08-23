/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
 * Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
 * Copyright 2017 Anton Anikin <anton@anikin.xyz>

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

#pragma once

#include <QAbstractTableModel>

namespace Valgrind
{

struct CachegrindFunction
{
    QString functionName;
    QStringList fileNames;

    QList<int> eventValues;
};

class CachegrindFunctionsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CachegrindFunctionsModel(QObject* parent = nullptr);
    ~CachegrindFunctionsModel() override;

    void setEventsList(const QStringList& events);
    void addItem(CachegrindFunction* item, bool isTotal = false);

    void setPercentageValues(bool value);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QList<CachegrindFunction*> m_items;
    CachegrindFunction* m_totalItem;

    bool m_percentageValues;

    QStringList m_eventList;
};

}
