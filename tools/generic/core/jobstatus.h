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

#pragma once

#include <interfaces/istatus.h>

#include <QObject>

namespace Valgrind
{

class IJob;

class JobStatus : public QObject, public KDevelop::IStatus
{
    Q_OBJECT
    Q_INTERFACES(KDevelop::IStatus)

public:
    explicit JobStatus(const IJob* job, const QString& jobName);
    ~JobStatus() override;

    QString statusName() const override;

signals:
    void clearMessage(KDevelop::IStatus*) override;
    void hideProgress(KDevelop::IStatus*) override;
    void showErrorMessage(const QString& message, int timeout = 0) override;
    void showMessage(KDevelop::IStatus*, const QString& message, int timeout = 0) override;
    void showProgress(KDevelop::IStatus*, int minimum, int maximum, int value) override;

protected:
    QString m_jobName;
};

}
