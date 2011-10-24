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

#include <massifitem.h>

namespace valgrind
{

  MassifModel::MassifModel(QObject * parent)
  {
    qDebug() << "Massif model Created";
  }

  MassifModel::~ MassifModel()
  {
    //    qDeleteAll(errors);
  }

  QVariant MassifModel::data ( const QModelIndex & index, int role ) const
  {
    qDebug() << "Massif call data !";

    if (!index.isValid())
      return QVariant();

    if (index.row() >= m_snapshots.size())
      return QVariant();

    //    if (role == Qt::DisplayRole)
    // return m_snapshots.at(index.row());
    return QVariant();
  }

  QVariant MassifModel::headerData(int section, Qt::Orientation orientation, int role) const
  {
    return QVariant();
  }

  int MassifModel::rowCount ( const QModelIndex & p ) const
  {
    return m_snapshots.count();
  }


  void MassifModel::newItem(ModelItem *i)
  {
    MassifItem *m = dynamic_cast<MassifItem *>(i);
    Q_ASSERT(m);
    qDebug() << "Massif new Element";
    m_snapshots << m;
  }

  void MassifModel::reset()
  {
    qDebug() << "Massif Reset";
    qDeleteAll(m_snapshots);
    m_snapshots.clear();
  }


  // useless if use QAbstractListModel
  QModelIndex MassifModel::index(int, int, const QModelIndex&) const
  {
    // qDebug() << "Massif index";
    // no index
    return QModelIndex();
  }

  QModelIndex MassifModel::parent(const QModelIndex&) const
  {
    // qDebug() << "Massif parent";
    // no parent
    return QModelIndex();
  }

  int MassifModel::columnCount(const QModelIndex&) const
  {
    // qDebug() << "Massif columnCount";
    // only one column
    return 1;
  }

}

#include "massifmodel.moc"

