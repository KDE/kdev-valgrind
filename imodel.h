/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>

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

#ifndef _IMODEL_H_
#define _IMODEL_H_

#include <QAbstractItemModel>


namespace valgrind
{

  class ModelItem
  {

  public:
    ModelItem(){};

    virtual ~ModelItem(){};

  };

  class Model: public QAbstractItemModel
  {
    Q_OBJECT

    public:

    Model(QObject* parent = 0);

    virtual ~Model();

    enum eElementType {
      startError,
      error,
      startFrame,
      frame,
      startStack,
      stack
    };

    public slots:

    /**
     * Compatibility with memcheck module, TO REMOVE
     * Reception of a new item in the model
     */
    virtual void newElement(valgrind::Model::eElementType);

    /**
     * Compatibility with memcheck module, to remove
     * Reception of a new item in the model
     * Has to be pure
     */
    virtual void newItem(ModelItem *); // = 0;

    /**
     * Reception of data to register to the current item
     * TO REMOVE
     */
    virtual void newData(valgrind::Model::eElementType, QString name, QString value);

    /**
     * Resets the model content
     */
    virtual void reset() = 0;

    signals:

    /**
     * emit this signal to alert other modules that the model has been updated
     */
    void modelChanged();

  };
}

#endif /* _IMODEL_H_ */
