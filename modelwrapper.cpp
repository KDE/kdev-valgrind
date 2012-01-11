/* This file is part of KDevelop
 * Copyright 2011 Lucas SARIE <lucas.sarie@gmail.com>

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

#include "modelwrapper.h"

#include "imodel.h"
#include "job.h"

namespace valgrind
{

ModelWrapper::ModelWrapper(Model *impl)
    : m_job(0)
{
    Q_ASSERT(impl);
    m_modelImplementation = impl;
}



ModelWrapper::~ModelWrapper()
{
}

void ModelWrapper::job(valgrind::Job * job)
{
    //TODO: Search an other way to that :
    //QObject::connect(job, SIGNAL(destroyed()),
    //        this->getQAbstractItemModel(), SLOT(jobDestroyed()));
    m_job = job;
}

valgrind::Job * ModelWrapper::job(void)
{
    return m_job;
}

void ModelWrapper::jobDestroyed(void)
{
    m_job = NULL;
}

void ModelWrapper::newElement(Model::eElementType type)
{
    m_modelImplementation->newElement(type);
}

void ModelWrapper::newData(Model::eElementType type, QString name, QString value)
{
    m_modelImplementation->newData(type, name, value);
}

void ModelWrapper::newItem(ModelItem *item)
{
    m_modelImplementation->newItem(item);
}

void ModelWrapper::reset()
{
    m_modelImplementation->reset();
}
}

#include "modelwrapper.moc"
