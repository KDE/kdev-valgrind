/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2016 Anton Anikin <anton.anikin@htower.ru>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#pragma once

#include <QAbstractItemModel>

namespace valgrind
{

class IJob;
class ModelWrapper;

class ModelItem
{
public:
    ModelItem() {};
    virtual ~ModelItem() {};
};

class Model
{
public:
    Model();
    virtual ~Model() {}

    enum eElementType {
        startError,
        error,
        startFrame,
        frame,
        startStack,
        stack
    };

    virtual QAbstractItemModel* getQAbstractItemModel(int n = 0);

    virtual void newElement(Model::eElementType) {}
    virtual void newItem(ModelItem*) {}
    virtual void newData(Model::eElementType, const QString&, const QString&) {}
    virtual void reset() {};

    void setModelWrapper(ModelWrapper* mdlw);
    ModelWrapper* getModelWrapper() const;
    IJob* job() const;

protected:
    ModelWrapper* m_modelWrapper;
};

}
