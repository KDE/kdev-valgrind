/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2009 Andreas Pakulat <apaku@gmx.de>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@buffout.org>

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

#include "valgrindgenericconfigpage.h"
#include "valgrindmemcheckconfigpage.h"
#include "valgrindcachegrindconfigpage.h"
#include "valgrindhelgrindconfigpage.h"
#include "valgrindcallgrindconfigpage.h"
#include "valgrindmassifconfigpage.h"
#include "valgrindlauncher.h"
#include "valgrindconfig.h"
#include "valgrindjob.h"

ValgrindLauncher::ValgrindLauncher(ValgrindPlugin *inst) : m_plugin(inst)
{
    // these are tabs in each menu
    factories << new ValgrindGenericConfigPageFactory(m_plugin);
    factories << new ValgrindMemcheckConfigPageFactory();
    factories << new ValgrindMassifConfigPageFactory();

    /*
    ** Those are unimplemented at the moment: see config/valgrindgenericconfigpage.cpp
    **	to enable them.
    */
    // factories << new ValgrindCachegrindConfigPageFactory();
    // factories << new ValgrindCallgrindConfigPageFactory();
    // factories << new ValgrindHelgrindConfigPageFactory();


}

KJob* ValgrindLauncher::start(const QString& launchMode, KDevelop::ILaunchConfiguration* cfg)
{
    Q_ASSERT(cfg);
    if( !cfg )
        return 0;
    qDebug() << "LAUNCHMODE" << launchMode;
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
        l << new ValgrindJob( cfg, m_plugin, KDevelop::ICore::self()->runController() );
        return new KDevelop::ExecuteCompositeJob( KDevelop::ICore::self()->runController(), l );
    }
    kWarning() << "Unknown launch mode " << launchMode << "for config:" << cfg->name();
    return 0;
}

void ValgrindLauncher::addMode(ValgrindLaunchMode* mode)
{
    if( !modes.contains( mode->id() ) )
    {
        modes.insert( mode->id(), mode );
    }
}

QStringList ValgrindLauncher::supportedModes() const
{
    return modes.keys(); // these are entries in menus
}

QList< KDevelop::LaunchConfigurationPageFactory* > ValgrindLauncher::configPages() const
{
    return factories;
}

QString ValgrindLauncher::description() const
{
    return i18n( "Profile application with Valgrind" );
}

QString ValgrindLauncher::id()
{
    return "valgrind";
}

QString ValgrindLauncher::name() const
{
    return i18n("Valgrind");
}
