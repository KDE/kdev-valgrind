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

#include "massifmodel.h"

#include <QApplication>

#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kglobalsettings.h>

#include <iostream>

namespace valgrind
{

  MassifModel::MassifModel(QObject * parent)
  {
  }

  MassifModel::~ MassifModel()
  {
    //    qDeleteAll(errors);
  }

  QVariant MassifModel::data ( const QModelIndex & index, int role ) const
  {
    qDebug() << "call data !";

    if (!index.isValid())
      return QVariant();

    if (index.row() >= m_list.size())
      return QVariant();

    if (role == Qt::DisplayRole)
      return m_list.at(index.row());
    else
      return QVariant();
  }

  QVariant MassifModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    qDebug() << "headerData";
    return QVariant();
  }

  int MassifModel::rowCount ( const QModelIndex & p ) const
  {
    qDebug() << "rowCount";
    return m_list.count();
  }


  void MassifModel::newElement(MassifModel::eElementType e)
  {
    qDebug() << "Massif new Element";
  }

  void MassifModel::newData(MassifModel::eElementType e, QString name, QString value)
  {
    qDebug() << "Massif new Data";// << name << value;

    beginInsertRows(QModelIndex(), m_list.size(), m_list.size() + 1);
    m_list << value;
    endInsertRows();
  }

  void MassifModel::reset()
  {
    qDebug() << "Massif Reset";
    //    qDeleteAll(errors);
    //    m_errors.clear();
    //    reset();
  }


  // useless if use QAbstractListModel
  QModelIndex MassifModel::index(int, int, const QModelIndex&) const
  {
    qDebug() << "index";
    // no index
    return QModelIndex();
  }

  QModelIndex MassifModel::parent(const QModelIndex&) const
  {
    qDebug() << "parent";
    // no parent
    return QModelIndex();
  }

  int MassifModel::columnCount(const QModelIndex&) const
  {
    qDebug() << "columnCount";
    // only one column
    return 1;
  }

}

#include "massifmodel.moc"

