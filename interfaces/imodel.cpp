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

#include "imodel.h"

#include "interfaces/ijob.h"
#include "modelwrapper.h"

namespace valgrind
{

Model::Model()
{
    m_modelWrapper = nullptr;
}

QAbstractItemModel* Model::getQAbstractItemModel(int)
{
    return nullptr;
}

void Model::setModelWrapper(ModelWrapper* mdl)
{
    m_modelWrapper = mdl;
}

ModelWrapper* Model::getModelWrapper() const
{
    return m_modelWrapper;
}

IJob* Model::job() const
{
    if (m_modelWrapper)
      return m_modelWrapper->job();

    return nullptr;
}

}
