/* This file is part of KDevelop
   Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING. If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "jobstatus.h"

#include "debug.h"
#include "ijob.h"

namespace Valgrind
{

JobStatus::JobStatus(const IJob* job, const QString& jobName)
    : m_jobName(jobName)
{
    Q_ASSERT(job);

    connect(job, &IJob::started, this, [this]() {
        emit showProgress(this, 0, 0, 0);
    });

    connect(job, &IJob::finished, this, [this]() {
        emit hideProgress(this);
    });
}

JobStatus::~JobStatus()
{
}

QString JobStatus::statusName() const
{
    return m_jobName;
}

}
