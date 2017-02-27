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
#include "widget.h"

#include "cachegrind/launcher.h"
#include "callgrind/launcher.h"
#include "drd/launcher.h"
#include "helgrind/launcher.h"
#include "massif/launcher.h"
#include "memcheck/launcher.h"

#include <execute/iexecuteplugin.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/iuicontroller.h>
#include <interfaces/launchconfigurationtype.h>
#include <kactioncollection.h>
#include <kpluginfactory.h>
#include <shell/core.h>
#include <shell/launchconfiguration.h>
#include <shell/runcontroller.h>

K_PLUGIN_FACTORY_WITH_JSON(ValgrindFactory, "kdevvalgrind.json",  registerPlugin<Valgrind::Plugin>();)

namespace Valgrind
{

class WidgetFactory : public KDevelop::IToolViewFactory
{
public:
    explicit WidgetFactory(Plugin* plugin)
        : m_plugin(plugin)
    {
    }

    QWidget* create(QWidget* parent = nullptr) override
    {
        return new Widget(m_plugin, parent);
    }

    Qt::DockWidgetArea defaultPosition() override
    {
        return Qt::BottomDockWidgetArea;
    }

    QString id() const override
    {
        return QStringLiteral("org.kdevelop.ValgrindView");
    }

private:
    Plugin* m_plugin;
};

Plugin::Plugin(QObject* parent, const QVariantList&)
    : IPlugin("kdevvalgrind", parent)
    , m_factory(new WidgetFactory(this))
    , m_launchMode(new LaunchMode)
    , m_problemModel(new ProblemModel(this))
{
    qCDebug(KDEV_VALGRIND) << "setting valgrind rc file";
    setXMLFile("kdevvalgrind.rc");

    core()->uiController()->addToolView(i18n("Valgrind"), m_factory);
    core()->runController()->addLaunchMode(m_launchMode);

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

    QAction* action = nullptr;

    action = new QAction(i18n("Memcheck: a memory error detector"), this);
    connect(action, &QAction::triggered, this, [this]() { executeDefaultLaunch(Memcheck::launcherId); });
    actionCollection()->addAction("memcheck_tool", action);

    action = new QAction(i18n("Cachegrind: a cache and branch-prediction profiler"), this);
    connect(action, &QAction::triggered, this, [this]() { executeDefaultLaunch(Cachegrind::launcherId); });
    actionCollection()->addAction("cachegrind_tool", action);

    action = new QAction(i18n("Callgrind: a call-graph generating cache and branch prediction profiler"), this);
    connect(action, &QAction::triggered, this, [this]() { executeDefaultLaunch(Callgrind::launcherId); });
    actionCollection()->addAction("callgrind_tool", action);

    action = new QAction(i18n("Helgrind: a thread error detector"), this);
    connect(action, &QAction::triggered, this, [this]() { executeDefaultLaunch(Helgrind::launcherId); });
    actionCollection()->addAction("helgrind_tool", action);

    action = new QAction(i18n("DRD: a thread error detector"), this);
    connect(action, &QAction::triggered, this, [this]() { executeDefaultLaunch(DRD::launcherId); });
    actionCollection()->addAction("drd_tool", action);

    action = new QAction(i18n("Massif: a heap profiler"), this);
    connect(action, &QAction::triggered, this, [this]() { executeDefaultLaunch(Massif::launcherId); });
    actionCollection()->addAction("massif_tool", action);
}

Plugin::~Plugin()
{
}

void Plugin::unload()
{
    core()->uiController()->removeToolView(m_factory);

    for (auto plugin : core()->pluginController()->allPluginsForExtension(QStringLiteral("org.kdevelop.IExecutePlugin"))) {
        setupExecutePlugin(plugin, false);
    }
    Q_ASSERT(m_launchers.isEmpty());

    core()->runController()->removeLaunchMode(m_launchMode);
    delete m_launchMode;
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
        KDevelop::ILauncher* launcher;

        launcher = new Memcheck::Launcher(this, m_launchMode);
        m_launchers.insert(plugin, launcher);
        type->addLauncher(launcher);

        launcher = new Cachegrind::Launcher(this, m_launchMode);
        m_launchers.insert(plugin, launcher);
        type->addLauncher(launcher);

        launcher = new Callgrind::Launcher(this, m_launchMode);
        m_launchers.insert(plugin, launcher);
        type->addLauncher(launcher);

        launcher = new Helgrind::Launcher(this, m_launchMode);
        m_launchers.insert(plugin, launcher);
        type->addLauncher(launcher);

        launcher = new DRD::Launcher(this, m_launchMode);
        m_launchers.insert(plugin, launcher);
        type->addLauncher(launcher);

        launcher = new Massif::Launcher(this, m_launchMode);
        m_launchers.insert(plugin, launcher);
        type->addLauncher(launcher);
    }

    else {
        for (auto launcher : m_launchers.values(plugin)) {
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

ProblemModel* Plugin::problemModel() const
{
    return m_problemModel;
}

void Plugin::jobReadyToStart(IJob* job)
{
    for (auto action : actionCollection()->actions()) {
        action->setEnabled(false);
    }

    Q_ASSERT(job);
    if (!job->hasView()) {
        // FIXME ugly temporary code; replace with ITool in the future
        if (job->tool() == "memcheck") {
            m_problemModel->reset("Memcheck");
        }
        else if (job->tool() == "helgrind") {
            m_problemModel->reset("Helgrind");
        }
        else if (job->tool() == "drd") {
            m_problemModel->reset("DRD");
        }
    }
}

void Plugin::jobReadyToFinish(IJob* job, bool ok)
{
    if (!ok) {
        return;
    }

    Q_ASSERT(job);
    if (job->hasView()) {
        addView(job->createView(), QStringLiteral("%1 (%2)").arg(job->target()).arg(job->tool()));
        core()->uiController()->findToolView("Valgrind", m_factory);
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
}

void Plugin::executeDefaultLaunch(const QString& launcherId)
{
    auto runController = KDevelop::Core::self()->runControllerInternal();
    Q_ASSERT(runController);

    auto defaultLaunch = runController->defaultLaunch();
    if (defaultLaunch) {
        defaultLaunch->setLauncherForMode(launchModeId, launcherId);
        runController->executeDefaultLaunch(launchModeId);
    }
}

}

#include "plugin.moc"
