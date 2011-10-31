/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>

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

#ifndef _MASSIFITEM_H_
#define _MASSIFITEM_H_

#include <QString>
#include <QMap>
#include <QStringList>

#include "imodel.h"

namespace valgrind
{

  class MassifItem : public ModelItem
  {

  public:

    MassifItem();
    MassifItem(bool);
    virtual ~MassifItem();

    enum Columns {
      Snapshot,
      Time,
      MemHeapB,
      MemHeapExtraB,
      MemStacksB
    };

    /*
     * Called when data related to the error has been parsed
     */
    void incomingData(const QString &name, const QString &value);
    void incomingAlloc(const QString &value);

    // use by the model
    void appendChild(MassifItem *child);
    void setParent(MassifItem *parent);
    MassifItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    MassifItem *parent();

  private:
    QMap<QString, QString> m_values;
    bool m_child;

    // use by the model
    QList<MassifItem*> m_childItems;
    MassifItem *m_parentItem;
  };

}

#endif /* _MASSIFITEM_H_ */
