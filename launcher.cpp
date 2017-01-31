/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2009 Andreas Pakulat <apaku@gmx.de>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
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

#include "launcher.h"

#include "cachegrind/configpage.h"
#include "callgrind/configpage.h"
#include "generic/configpage.h"
#include "massif/configpage.h"
#include "memcheck/configpage.h"

#include "config.h"
#include "debug.h"
#include "interfaces/ijob.h"

#include <execute/iexecuteplugin.h>
#include <interfaces/icore.h>
#include <interfaces/ilaunchconfiguration.h>
#include <interfaces/iplugincontroller.h>
#include <klocalizedstring.h>
#include <util/executecompositejob.h>

namespace valgrind
{

Launcher::Launcher(Plugin *inst)
    : m_plugin(inst)
{
    // these are tabs in each menu
    m_factories << new GenericConfigPageFactory();
    m_factories << new MemcheckConfigPageFactory();
    m_factories << new MassifConfigPageFactory();
    m_factories << new CachegrindConfigPageFactory();
    m_factories << new CallgrindConfigPageFactory();

    /*
    ** Those are unimplemented at the moment: see config/genericconfigpage.cpp
    **  to enable them.
    */
//     factories << new ValgrindHelgrindConfigPageFactory();
}

KJob* Launcher::start(const QString& launchMode, KDevelop::ILaunchConfiguration* cfg)
{
    Q_ASSERT(cfg);
    if (!cfg)
        return nullptr;

    if (m_modes.contains(launchMode)) {
        IExecutePlugin* iface = KDevelop::ICore::self()->pluginController()->pluginForExtension("org.kdevelop.IExecutePlugin")->extension<IExecutePlugin>();
        Q_ASSERT(iface);

        QList<KJob*> l;
        KJob* depjob = iface->dependencyJob(cfg);
        if (depjob)
            l << depjob;
        l << IJob::createToolJob(cfg, m_plugin, KDevelop::ICore::self()->runController());

        return new KDevelop::ExecuteCompositeJob(KDevelop::ICore::self()->runController(), l);
    }

    qCWarning(KDEV_VALGRIND) << "Unknown launch mode " << launchMode << "for config:" << cfg->name();

    return nullptr;
}

void Launcher::addMode(LaunchMode* mode)
{
    if (!m_modes.contains(mode->id()))
        m_modes.insert(mode->id(), mode);
}

QStringList Launcher::supportedModes() const
{
    return m_modes.keys(); // these are entries in menus
}

QList<KDevelop::LaunchConfigurationPageFactory*> Launcher::configPages() const
{
    return m_factories;
}

QString Launcher::description() const
{
    return i18n("Profile application with Valgrind");
}

QString Launcher::id()
{
    return "valgrind";
}

QString Launcher::name() const
{
    return i18n("Valgrind");
}

}
