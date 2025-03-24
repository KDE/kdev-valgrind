/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2009 Andreas Pakulat <apaku@gmx.de>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
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

#include "launcher.h"

#include "common_configpage.h"
#include "job.h"
#include "launchmode.h"
#include "plugin.h"
#include "tool.h"

#include <execute/iexecuteplugin.h>
#include <interfaces/icore.h>
#include <interfaces/iruncontroller.h>
#include <util/executecompositejob.h>

namespace Valgrind
{

Launcher::Launcher(const Tool* tool, const IExecutePlugin& execute)
    : m_tool(tool)
    , m_execute(execute)
{
    Q_ASSERT(tool);

    m_configPageFactories += tool->createConfigPageFactory();
    m_configPageFactories += new CommonConfigPageFactory;
}

Launcher::~Launcher()
{
    qDeleteAll(m_configPageFactories);
}

KJob* Launcher::start(const QString& launchMode, KDevelop::ILaunchConfiguration* launchConfig)
{
    Q_ASSERT(launchConfig);

    if (Plugin::self()->launchMode()->id() != launchMode) {
        return nullptr;
    }

    QList<KJob*> jobList;
    if (auto* const depJob = m_execute.dependencyJob(launchConfig)) {
        jobList += depJob;
    }

    auto* const valgrindJob = m_tool->createJob({m_execute, *launchConfig});
    jobList += valgrindJob;

    auto ecJob = new KDevelop::ExecuteCompositeJob(KDevelop::ICore::self()->runController(), jobList);
    ecJob->setObjectName(valgrindJob->objectName());

    return ecJob;
}

QStringList Launcher::supportedModes() const
{
    return { Plugin::self()->launchMode()->id() };
}

QList<KDevelop::LaunchConfigurationPageFactory*> Launcher::configPages() const
{
    return m_configPageFactories;
}

QString Launcher::name() const
{
    return m_tool->name();
}

QString Launcher::description() const
{
    return m_tool->fullName();
}

QString Launcher::id()
{
    return m_tool->id();
}

}
