/* This file is part of KDevelop
   Copyright 2002 Harald Fernengel <harry@kdevelop.org>
   Copyright 2007 Hamish Rodda <rodda@kde.org>
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

#include "plugin.h"

#include "globalconfigpage.h"

#include "debug.h"
#include "job.h"
#include "launchmode.h"
#include "problemmodel.h"
#include "toolviewfactory.h"

#include "tools/cachegrind/cachegrind_tool.h"
#include "tools/callgrind/callgrind_tool.h"
#include "tools/drd/drd_tool.h"
#include "tools/helgrind/helgrind_tool.h"
#include "tools/massif/massif_tool.h"
#include "tools/memcheck/memcheck_tool.h"

#include <execute/iexecuteplugin.h>
#include <interfaces/ilauncher.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/launchconfigurationtype.h>
#include <qtcompat_p.h>
#include <shell/core.h>
#include <shell/launchconfiguration.h>
#include <shell/runcontroller.h>

#include <KActionCollection>
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(ValgrindFactory, "kdevvalgrind.json",  registerPlugin<Valgrind::Plugin>();)

namespace Valgrind
{

Plugin* Plugin::m_self = nullptr;

Plugin::Plugin(QObject* parent, const KPluginMetaData& metaData, const QVariantList&)
    : IPlugin(QStringLiteral("kdevvalgrind"), parent, metaData)
    , m_toolViewFactory(new ToolViewFactory)
    , m_launchMode(new LaunchMode)
    , m_problemModel(new ProblemModel)
    , m_isRunning(false)
{
    m_self = this;
    setXMLFile(QStringLiteral("kdevvalgrind.rc"));

    core()->uiController()->addToolView(i18n("Valgrind"), m_toolViewFactory);
    core()->runController()->addLaunchMode(m_launchMode.data());

    m_tools += MemcheckTool::self();
    m_tools += CachegrindTool::self();
    m_tools += CallgrindTool::self();
    m_tools += HelgrindTool::self();
    m_tools += DrdTool::self();
    m_tools += MassifTool::self();

    for (auto tool : std::as_const(m_tools)) {
        auto action = new QAction(i18n("Run %1", tool->fullName()), this);
        connect(action, &QAction::triggered, this, [this, tool]() {
                executeDefaultLaunch(tool->id());
        });
        actionCollection()->addAction(tool->menuActionName(), action);
    }

    auto pluginController = core()->pluginController();
    const auto plugins = pluginController->allPluginsForExtension(QStringLiteral("org.kdevelop.IExecutePlugin"));
    for (auto plugin : plugins) {
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
    m_problemModel.reset(nullptr);

    const auto plugins = core()->pluginController()->allPluginsForExtension(QStringLiteral("org.kdevelop.IExecutePlugin"));
    for (auto plugin : plugins) {
        setupExecutePlugin(plugin, false);
    }
    Q_ASSERT(m_launchers.isEmpty());

    core()->runController()->removeLaunchMode(m_launchMode.data());
    m_launchMode.reset(nullptr);

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
        auto& launchers = m_launchers[plugin];
        Q_ASSERT(launchers.empty());
        launchers.reserve(m_tools.size());
        for (auto tool : std::as_const(m_tools)) {
            auto* const launcher = tool->createLauncher(*iface);
            launchers.push_back(launcher);
            type->addLauncher(launcher);
        }
    }

    else {
        const auto it = m_launchers.constFind(plugin);
        Q_ASSERT(it != m_launchers.cend());
        for (auto* const launcher : std::as_const(*it)) {
            Q_ASSERT(launcher);
            type->removeLauncher(launcher);
            delete launcher;
        }
        m_launchers.erase(it);
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
    return m_launchMode.data();
}

ProblemModel* Plugin::problemModel() const
{
    return m_problemModel.data();
}

void Plugin::jobReadyToStart(Job* job)
{
    m_isRunning = true;

    const auto actions = actionCollection()->actions();
    for (auto action : actions) {
        action->setEnabled(false);
    }

    Q_ASSERT(job);
    if (!job->tool()->hasView()) {
        m_problemModel->reset(job->tool());
    }
}

void Plugin::jobReadyToFinish(Job* job, bool ok)
{
    if (!ok) {
        return;
    }

    Q_ASSERT(job);
    if (job->tool()->hasView()) {
        Q_EMIT addView(job->createView(), job->title());

        core()->uiController()->findToolView(i18n("Valgrind"), m_toolViewFactory);
    } else {
        m_problemModel->show();
    }
}

void Plugin::jobFinished(KJob* job)
{
    Q_UNUSED(job);

    const auto actions = actionCollection()->actions();
    for (auto action : actions) {
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
