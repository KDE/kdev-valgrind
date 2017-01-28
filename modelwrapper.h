/* This file is part of KDevelop
   Copyright 2011 Lucas SARIE <lucas.sarie@gmail.com>
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

#include "interfaces/imodel.h"

namespace valgrind
{

/**
 * This class is used to wrap signal
 * It has been created because there was problems with multiple QObject inheritence
 * Before Model inherit QAbstractItemModel but there was problem when a tool has many model
 * That why this class exists
 */
class ModelWrapper :  public QObject
{
  Q_OBJECT

public:
    explicit ModelWrapper(IModel* impl);
    ~ModelWrapper() override;

public slots:
    /**
     * Set the associated process
     */
    void job(IJob* job);

    /**
     * Get the associated process
     */
    IJob* job();

    void jobDestroyed();

signals:
    /**
     * emit this signal to alert other modules that the model has been updated
     */
    void modelChanged();

private:
    IJob* m_job;
    IModel* m_modelImplementation;
};

}
