/* This file is part of KDevelop
   Copyright 2002 Harald Fernengel <harry@kdevelop.org>
   Copyright 2007 Hamish Rodda <rodda@kde.org>
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

#include "plugin.h"

#include "config/globalconfigpage.h"

#include "debug.h"
#include "ijob.h"
#include "launchmode.h"
#include "problemmodel.h"
#include "toolviewfactory.h"

#include "cachegrind/tool.h"
#include "callgrind/tool.h"
#include "drd/tool.h"
#include "helgrind/tool.h"
#include "massif/tool.h"
#include "memcheck/tool.h"

#include <execute/iexecuteplugin.h>
#include <interfaces/ilauncher.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/launchconfigurationtype.h>
#include <kactioncollection.h>
#include <kpluginfactory.h>
#include <shell/core.h>
#include <shell/launchconfiguration.h>
#include <shell/runcontroller.h>

K_PLUGIN_FACTORY_WITH_JSON(ValgrindFactory, "kdevvalgrind.json",  registerPlugin<Valgrind::Plugin>();)

namespace Valgrind
{

Plugin* Plugin::m_self = nullptr;

Plugin::Plugin(QObject* parent, const QVariantList&)
    : IPlugin("kdevvalgrind", parent)
    , m_toolViewFactory(new ToolViewFactory)
    , m_launchMode(new LaunchMode)
    , m_problemModel(new ProblemModel)
    , m_isRunning(false)
{
    m_self = this;

    qCDebug(KDEV_VALGRIND) << "setting valgrind rc file";
    setXMLFile("kdevvalgrind.rc");

    core()->uiController()->addToolView(i18n("Valgrind"), m_toolViewFactory);
    core()->runController()->addLaunchMode(m_launchMode);

    m_tools += Memcheck::Tool::self();
    m_tools += Cachegrind::Tool::self();
    m_tools += Callgrind::Tool::self();
    m_tools += Helgrind::Tool::self();
    m_tools += DRD::Tool::self();
    m_tools += Massif::Tool::self();

    for (auto tool : m_tools) {
        auto action = new QAction(tool->fullName(), this);
        connect(action, &QAction::triggered, this, [this, tool]() {
                executeDefaultLaunch(tool->id());
        });
        actionCollection()->addAction(tool->menuActionName(), action);
    }

    auto pluginController = core()->pluginController();
    for (auto plugin : pluginController->allPluginsForExtension(QStringLiteral("org.kdevelop.IExecutePlugin"))) {
        setupExecutePlugin(plugin, true);
    }

    connect(pluginController, &KDevelop::IPluginController::pluginLoaded,
            this, [this](KDevelop::IPlugin* plugin) {
                setupExecutePlugin(plugin, true);
            });

    connect(pluginController, &KDevelop::IPluginController::unloadingPlugin,
            this, [this](KDevelop::IPlugin* plugin) {
                setupExecutePlugin(plugin, false);
            });
}

Plugin::~Plugin()
{
    m_self = nullptr;
}

Plugin* Plugin::self()
{
    return m_self;
}

void Plugin::unload()
{
    core()->uiController()->removeToolView(m_toolViewFactory);

    for (auto plugin : core()->pluginController()->allPluginsForExtension(QStringLiteral("org.kdevelop.IExecutePlugin"))) {
        setupExecutePlugin(plugin, false);
    }
    Q_ASSERT(m_launchers.isEmpty());

    core()->runController()->removeLaunchMode(m_launchMode);
    delete m_launchMode;

    qDeleteAll(m_tools);
}

void Plugin::setupExecutePlugin(KDevelop::IPlugin* plugin, bool load)
{
    if (plugin == this) {
        return;
    }

    auto iface = plugin->extension<IExecutePlugin>();
    if (!iface) {
        return;
    }

    auto type = core()->runController()->launchConfigurationTypeForId(iface->nativeAppConfigTypeId());
    Q_ASSERT(type);

    if (load) {
        for (auto tool : m_tools) {
            auto launcher = tool->createLauncher();
            m_launchers.insert(plugin, launcher);
            type->addLauncher(launcher);
        }
    }

    else {
        auto pluginLaunchers = m_launchers.values(plugin);
        for (auto launcher : pluginLaunchers) {
            Q_ASSERT(launcher);

            m_launchers.remove(plugin, launcher);
            type->removeLauncher(launcher);
            delete launcher;
        }
    }
}

int Plugin::configPages() const
{
    return 1;
}

KDevelop::ConfigPage* Plugin::configPage(int number, QWidget* parent)
{
    if (number) {
        return nullptr;
    }

    return new GlobalConfigPage(this, parent);
}

Valgrind::LaunchMode* Plugin::launchMode() const
{
    return m_launchMode;
}

ProblemModel* Plugin::problemModel() const
{
    return m_problemModel;
}

void Plugin::jobReadyToStart(IJob* job)
{
    m_isRunning = true;
    for (auto action : actionCollection()->actions()) {
        action->setEnabled(false);
    }

    Q_ASSERT(job);
    if (!job->tool()->hasView()) {
        m_problemModel->reset(job->tool());
    }
}

void Plugin::jobReadyToFinish(IJob* job, bool ok)
{
    if (!ok) {
        return;
    }

    Q_ASSERT(job);
    if (job->tool()->hasView()) {
        emit addView(job->createView(), job->statusName());

        core()->uiController()->findToolView(i18n("Valgrind"), m_toolViewFactory);
    } else {
        m_problemModel->show();
    }
}

void Plugin::jobFinished(KJob* job)
{
    Q_UNUSED(job);
    for (auto action : actionCollection()->actions()) {
        action->setEnabled(true);
    }
    m_isRunning = false;
}

bool Plugin::isRunning()
{
    return m_isRunning;
}

void Plugin::executeDefaultLaunch(const QString& launcherId)
{
    auto runController = KDevelop::Core::self()->runControllerInternal();
    Q_ASSERT(runController);

    if (runController->launchConfigurations().isEmpty()) {
        runController->showConfigurationDialog();
    }

    auto defaultLaunch = runController->defaultLaunch();
    if (defaultLaunch) {
        defaultLaunch->setLauncherForMode(m_launchMode->id(), launcherId);
        runController->executeDefaultLaunch(m_launchMode->id());
    }
}

}

#include "plugin.moc"
