/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2016-2017 Anton Anikin <anton@anikin.xyz>

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

#include "job.h"

namespace Valgrind
{

class XmlConfig;

class XmlJob : public Job
{
    Q_OBJECT

public:
    ~XmlJob() override;

protected:
    explicit XmlJob(const Tool* tool, const LaunchInfo& launchInfo, XmlConfig* config);

    QWidget* createView() override final;

    void processValgrindOutput(const QStringList& lines) override final;

    bool processEnded() override final;

    void addLoggingArgs(QStringList& args) const override final;
    void addToolArgs(QStringList& args) const override final;

private:
    QScopedPointer<XmlConfig> m_config;
    QStringList m_xmlOutput;
};

}
