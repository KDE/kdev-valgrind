/* This file is part of KDevelop
   Copyright 2002 Harald Fernengel <harry@kdevelop.org>
   Copyright 2007 Hamish Rodda <rodda@kde.org>
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

#include "plugin.h"

#include "config/globalconfigpage.h"

#include "debug.h"
#include "generic/job.h"
#include "launcher.h"
#include "widget.h"

#include <execute/iexecuteplugin.h>
#include <interfaces/icore.h>
#include <interfaces/ilanguagecontroller.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/iuicontroller.h>
#include <interfaces/launchconfigurationtype.h>
#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <shell/problemmodel.h>
#include <shell/problemmodelset.h>

K_PLUGIN_FACTORY_WITH_JSON(ValgrindFactory, "kdevvalgrind.json",  registerPlugin<valgrind::Plugin>();)

namespace valgrind
{

static const QString modelId = QStringLiteral("Valgrind");

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
    , m_problemModel(new KDevelop::ProblemModel(this))
{
    qCDebug(KDEV_VALGRIND) << "setting valgrind rc file";
    setXMLFile("kdevvalgrind.rc");

    core()->uiController()->addToolView("Valgrind", m_factory);

    QAction* act = new QAction(i18n("Valgrind (Current Launch Configuration)"), this);
    connect(act, &QAction::triggered, this, &Plugin::runValgrind);
    actionCollection()->addAction("valgrind_generic", act);

    auto mode = new LaunchMode();
    core()->runController()->addLaunchMode(mode);

    // Add launcher for native apps
    IExecutePlugin* iface = core()->pluginController()->pluginForExtension("org.kdevelop.IExecutePlugin")->extension<IExecutePlugin>();
    Q_ASSERT(iface);

    auto launcher = new Launcher(this, mode);
    auto type = core()->runController()->launchConfigurationTypeForId(iface->nativeAppConfigTypeId());
    Q_ASSERT(type);

    type->addLauncher(launcher);

    m_problemModel->setFeatures(
        KDevelop::ProblemModel::ScopeFilter |
        KDevelop::ProblemModel::SeverityFilter |
        KDevelop::ProblemModel::Grouping |
        KDevelop::ProblemModel::CanByPassScopeFilter);

    KDevelop::ProblemModelSet* pms = core()->languageController()->problemModelSet();
    pms->addModel(modelId, i18n("Valgrind"), m_problemModel);
}

int Plugin::configPages() const
{
    return 1;
}

KDevelop::ConfigPage* Plugin::configPage(int number, QWidget* parent)
{
    if (number != 0) {
        return nullptr;
    }

    return new GlobalConfigPage(this, parent);
}

Plugin::~Plugin()
{
    KDevelop::ProblemModelSet* pms = core()->languageController()->problemModelSet();
    pms->removeModel(modelId);

    core()->uiController()->removeToolView(m_factory);
}

void Plugin::runValgrind()
{
    core()->runController()->executeDefaultLaunch(launchModeId);
}

void Plugin::jobReadyToStart(Generic::Job* job)
{
    Q_ASSERT(job);
    if (!job->hasView()) {
        m_problemModel->clearProblems();
    }
}

void Plugin::jobFinished(Generic::Job* job, bool ok)
{
    Q_ASSERT(job);
    if (!ok) {
        return;
    }

    if (job->hasView()) {
        addView(job->createView(), QStringLiteral("%1 (%2)").arg(job->target()).arg(job->tool()));
        core()->uiController()->findToolView("Valgrind", m_factory);
    } else {
        core()->languageController()->problemModelSet()->showModel(modelId);
    }
}

KDevelop::ProblemModel* Plugin::problemModel() const
{
    return m_problemModel;
}

}

#include "plugin.moc"
