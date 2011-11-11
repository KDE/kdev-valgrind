/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2009 Andreas Pakulat <apaku@gmx.de>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>

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

#include <kicon.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kconfiggroup.h>
#include <kparts/mainwindow.h>

#include <interfaces/icore.h>
#include <interfaces/iruncontroller.h>
#include <interfaces/ilaunchconfiguration.h>
#include <interfaces/iprojectcontroller.h>
#include <interfaces/iuicontroller.h>
#include <interfaces/iproject.h>
#include <project/projectmodel.h>
#include <project/builderjob.h>
#include <project/interfaces/iprojectbuilder.h>
#include <project/interfaces/ibuildsystemmanager.h>
#include <interfaces/iplugincontroller.h>
#include <util/executecompositejob.h>

#include <execute/iexecuteplugin.h>

#include "genericconfigpage.h"
#include "memcheckconfigpage.h"
#include "cachegrindconfigpage.h"
#include "helgrindconfigpage.h"
#include "callgrindconfigpage.h"
#include "massifconfigpage.h"
#include "launcher.h"
#include "config.h"
#include "job.h"

namespace valgrind
{

Launcher::Launcher(valgrind::Plugin *inst) : m_plugin(inst)
{
    // these are tabs in each menu
    factories << new valgrind::GenericConfigPageFactory(m_plugin);
    factories << new valgrind::MemcheckConfigPageFactory();
    factories << new valgrind::MassifConfigPageFactory();
    factories << new valgrind::CachegrindConfigPageFactory();

    /*
    ** Those are unimplemented at the moment: see config/genericconfigpage.cpp
    **	to enable them.
    */
    // factories << new ValgrindCallgrindConfigPageFactory();
    // factories << new ValgrindHelgrindConfigPageFactory();
}

KJob* Launcher::start(const QString& launchMode, KDevelop::ILaunchConfiguration* cfg)
{
    Q_ASSERT(cfg);
    if( !cfg )
        return 0;
    if( modes.contains( launchMode ) )
    {
        IExecutePlugin* iface = KDevelop::ICore::self()->pluginController()->pluginForExtension("org.kdevelop.IExecutePlugin")->extension<IExecutePlugin>();
        Q_ASSERT(iface);

        QList<KJob*> l;
        KJob* depjob = iface->dependecyJob( cfg );
        if( depjob )
        {
            l << depjob;
        }
        l << valgrind::Job::createToolJob(cfg, m_plugin, KDevelop::ICore::self()->runController());
        return new KDevelop::ExecuteCompositeJob( KDevelop::ICore::self()->runController(), l );
    }
    kWarning() << "Unknown launch mode " << launchMode << "for config:" << cfg->name();
    return 0;
}

void Launcher::addMode(valgrind::LaunchMode* mode)
{
    if( !modes.contains( mode->id() ) )
    {
        modes.insert( mode->id(), mode );
    }
}

QStringList Launcher::supportedModes() const
{
    return modes.keys(); // these are entries in menus
}

QList< KDevelop::LaunchConfigurationPageFactory* > Launcher::configPages() const
{
    return factories;
}

QString Launcher::description() const
{
    return i18n( "Profile application with Valgrind" );
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
