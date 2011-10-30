/* This file is part of KDevelop
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

#include "cachegrindmodel.h"

#include <QApplication>

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kglobalsettings.h>

#include <massifitem.h>

namespace valgrind
{

    CachegrindModel::CachegrindModel(QObject * parent)
    {
	Q_UNUSED(parent);
    }

    CachegrindModel::~CachegrindModel()
    {
    }

    void CachegrindModel::reset()
    {
    }



    QModelIndex CachegrindModel::index(int row, int column, const QModelIndex &parent) const
    {
	return QModelIndex();
    }

    QModelIndex CachegrindModel::parent(const QModelIndex &index) const
    {
	return QModelIndex();
    }

    int CachegrindModel::rowCount(const QModelIndex &parent) const
    {
	return 0;
    }

    int CachegrindModel::columnCount(const QModelIndex &parent) const
    {
	return 0;
    }

    QVariant CachegrindModel::data(const QModelIndex & index, int role) const
    {
	return QVariant();
    }

    QVariant CachegrindModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
	return QVariant();
    }


}

#include "cachegrindmodel.moc"

