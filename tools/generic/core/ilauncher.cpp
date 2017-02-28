/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2009 Andreas Pakulat <apaku@gmx.de>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2016-2017 Anton Anikin <anton.anikin@htower.ru>

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

#include "ilauncher.h"

#include "configpage.h"
#include "debug.h"
#include "ijob.h"
#include "launchmode.h"
#include "plugin.h"

#include <execute/iexecuteplugin.h>
#include <interfaces/icore.h>
#include <interfaces/iplugincontroller.h>
#include <klocalizedstring.h>
#include <util/executecompositejob.h>

namespace Valgrind
{

ILauncher::ILauncher(
    const QString& name,
    const QString& description,
    const QString& id,
    KDevelop::LaunchConfigurationPageFactory* configPageFactory)

    : m_name(name)
    , m_description(description)
    , m_id(id)
{
    Q_ASSERT(configPageFactory);

    m_configPageFactories += configPageFactory;
    m_configPageFactories += new Generic::ConfigPageFactory;
}

ILauncher::~ILauncher()
{
    qDeleteAll(m_configPageFactories);
}

KJob* ILauncher::start(const QString& launchMode, KDevelop::ILaunchConfiguration* config)
{
    Q_ASSERT(config);

    if (Plugin::self()->launchMode()->id() != launchMode) {
        return nullptr;
    }

    auto iface = KDevelop::ICore::self()->pluginController()->pluginForExtension("org.kdevelop.IExecutePlugin")->extension<IExecutePlugin>();
    Q_ASSERT(iface);

    QList<KJob*> jobList;
    if (KJob* depJob = iface->dependencyJob(config)) {
        jobList += depJob;
    }

    auto valgrindJob = createJob(config);
    jobList += valgrindJob;

    auto ecJob = new KDevelop::ExecuteCompositeJob(KDevelop::ICore::self()->runController(), jobList);
    ecJob->setObjectName(i18n("%1 Analysis (%2)", valgrindJob->tool(), valgrindJob->target()));

    return ecJob;
}

QStringList ILauncher::supportedModes() const
{
    return { Plugin::self()->launchMode()->id() };
}

QList<KDevelop::LaunchConfigurationPageFactory*> ILauncher::configPages() const
{
    return m_configPageFactories;
}

QString ILauncher::name() const
{
    return m_name;
}

QString ILauncher::description() const
{
    return m_description;
}

QString ILauncher::id()
{
    return m_id;
}

}
