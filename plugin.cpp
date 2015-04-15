/* This file is part of KDevelop
 *  Copyright 2002 Harald Fernengel <harry@kdevelop.org>
 *  Copyright 2007 Hamish Rodda <rodda@kde.org>

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

#include <unistd.h>

#include <QRegExp>
#include <QFile>
#include <QTreeView>
#include <QXmlInputSource>
#include <QXmlSimpleReader>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDomElement>
#include <QApplication>
#include <QAction>

#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kaboutdata.h>

#include <execute/iexecuteplugin.h>

#include <interfaces/icore.h>
#include <interfaces/iuicontroller.h>
#include <interfaces/iruncontroller.h>
#include <interfaces/launchconfigurationtype.h>
#include <interfaces/iplugincontroller.h>

#include "plugin.h"
#include "marks.h"
#include "memcheckmodel.h"
#include "job.h"
#include "config.h"
#include "widget.h"
#include "launcher.h"

#include "debug.h"

using namespace KDevelop;

K_PLUGIN_FACTORY_WITH_JSON(ValgrindFactory, "kdevvalgrind.json",  registerPlugin<valgrind::Plugin>();)

namespace valgrind
{

class WidgetFactory : public KDevelop::IToolViewFactory
{
public:
    WidgetFactory(valgrind::Plugin* plugin)
        : m_plugin(plugin) {
    }

    virtual QWidget* create(QWidget *parent = 0) {
        return new valgrind::Widget(m_plugin, parent);
    }

    virtual Qt::DockWidgetArea defaultPosition() {
        return Qt::BottomDockWidgetArea;
    }

    virtual QString id() const {
        return "org.kdevelop.ValgrindView";
    }

private:
    valgrind::Plugin* m_plugin;
};

Plugin::Plugin(QObject *parent, const QVariantList&)
    : IPlugin("kdevvalgrind", parent)
    , m_factory(new valgrind::WidgetFactory(this))
    , m_marks(new valgrind::Marks(this))
{

    qCDebug(KDEV_VALGRIND) << "setting valgrind rc file";
    setXMLFile("kdevvalgrind.rc");

    core()->uiController()->addToolView(i18n("Valgrind"), m_factory);
    // Initialize actions for the launch modes
    QAction* act;

    act = actionCollection()->addAction("valgrind_generic", this, SLOT(runValgrind()));
    act->setStatusTip(i18n("Launches the currently selected launch configuration with the Valgrind presets"));
    act->setText(i18n("Valgrind launch"));

    IExecutePlugin* iface = KDevelop::ICore::self()->pluginController()->pluginForExtension("org.kdevelop.IExecutePlugin")->extension<IExecutePlugin>();
    Q_ASSERT(iface);

    // Initialize launch modes and register them with platform, also put them into our launcher
    valgrind::Launcher* launcher = new valgrind::Launcher(this);

    valgrind::LaunchMode* mode = new valgrind::GenericLaunchMode();
    KDevelop::ICore::self()->runController()->addLaunchMode(mode);
    launcher->addMode(mode);
    //    KDevelop::ICore::self()->runController()->setDefaultLaunch( mode );

    // Add launcher for native apps
    KDevelop::LaunchConfigurationType* type = core()->runController()->launchConfigurationTypeForId(iface->nativeAppConfigTypeId());
    Q_ASSERT(type);
    type->addLauncher(launcher);

}

void Plugin::unload()
{
    core()->uiController()->removeToolView(m_factory);
}

void Plugin::incomingModel(valgrind::Model *model)
{
    emit newModel(model);
}

Plugin::~Plugin()
{
}

void Plugin::runValgrind()
{
    core()->runController()->executeDefaultLaunch("valgrind_generic");
}

void Plugin::loadOutput()
{
}

}

#include "plugin.moc"
