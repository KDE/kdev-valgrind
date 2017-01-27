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

#include "config.h"
#include "debug.h"
#include "interfaces/ijob.h"
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
    , m_model(new KDevelop::ProblemModel(this))
{
    qCDebug(KDEV_VALGRIND) << "setting valgrind rc file";
    setXMLFile("kdevvalgrind.rc");

    core()->uiController()->addToolView("Valgrind", m_factory);

    QAction* act = new QAction(i18n("Valgrind (Current Launch Configuration)"), this);
    connect(act, &QAction::triggered, this, &Plugin::runValgrind);
    actionCollection()->addAction("valgrind_generic", act);

    IExecutePlugin* iface = core()->pluginController()->pluginForExtension("org.kdevelop.IExecutePlugin")->extension<IExecutePlugin>();
    Q_ASSERT(iface);

    LaunchMode* mode = new GenericLaunchMode();
    core()->runController()->addLaunchMode(mode);
//     core()->runController()->setDefaultLaunch( mode );

    Launcher* launcher = new Launcher(this);
    launcher->addMode(mode);

    // Add launcher for native apps
    KDevelop::LaunchConfigurationType* type = core()->runController()->launchConfigurationTypeForId(iface->nativeAppConfigTypeId());
    Q_ASSERT(type);
    type->addLauncher(launcher);

    m_model->setFeatures(
        KDevelop::ProblemModel::ScopeFilter |
        KDevelop::ProblemModel::SeverityFilter |
        KDevelop::ProblemModel::Grouping |
        KDevelop::ProblemModel::CanByPassScopeFilter);

    KDevelop::ProblemModelSet* pms = core()->languageController()->problemModelSet();
    pms->addModel(modelId, i18n("Valgrind"), m_model.data());
}

KDevelop::ConfigPage* Plugin::configPage(int number, QWidget* parent)
{
    if (number != 0)
        return nullptr;
    else
        return new GlobalConfigPage(this, parent);
}

void Plugin::incomingModel(IModel* model)
{
    emit newModel(model);
}

Plugin::~Plugin()
{
    KDevelop::ProblemModelSet* pms = core()->languageController()->problemModelSet();
    pms->removeModel(modelId);

    core()->uiController()->removeToolView(m_factory);
}

void Plugin::runValgrind()
{
    m_model->clearProblems();
    core()->runController()->executeDefaultLaunch("valgrind_generic");
}

void Plugin::jobFinished(KJob* kjob)
{
    IJob* job = dynamic_cast<IJob*>(kjob);
    if (job && !job->error()) {
        if (job->tool() == QStringLiteral("memcheck"))
            core()->languageController()->problemModelSet()->showModel(modelId);
        else
            core()->uiController()->findToolView("Valgrind", m_factory);
    }
}

}

#include "plugin.moc"
