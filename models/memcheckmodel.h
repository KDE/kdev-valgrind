/* This file is part of KDevelop
 * Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
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

#ifndef _MEMCHECKMODEL_H_
#define _MEMCHECKMODEL_H_


#include <QHash>
#include <QStack>

#include "imodel.h"
#include "memcheckitems.h"

namespace valgrind
{

  class MemcheckError;


  /**
   * A class that represents the item model
   * \author Hamish Rodda \<rodda@kde.org\>
   */
  class MemcheckModel : public valgrind::Model, public MemcheckItem
  {
    Q_OBJECT

  public:

    MemcheckModel(QObject* parent = 0);

    virtual ~MemcheckModel();

    enum Columns {
      //Index = 0,
      Function,
      Source,
      Object
    };

    static const int numColumns = 1;

    // Item
    virtual MemcheckItem* parent() const { return 0L; }

    // Model
    QModelIndex indexForItem(MemcheckItem* item, int column = 0) const;
    MemcheckItem* itemForIndex(const QModelIndex& index) const;

    virtual void incomingData(QString name, QString value);
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent ( const QModelIndex & index ) const;
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

    void newStack();
    void newFrame();
    void newStartError();

    public slots:

    /**
     * Reception of a new item in the model
     */
    virtual void newElement(valgrind::Model::eElementType);

    /**
     * Reception of data to register to the current item
     */
    virtual void newData(valgrind::Model::eElementType, QString name, QString value);

    /**
     * Resets the model content
     */
    void reset();

    /**
     * Reception of a new item
     */
    virtual void newItem(valgrind::ModelItem *);

  private:
    QList<MemcheckError *> m_errors;

  };

}
#endif /* _MEMCHECKMODEL_H_ */
