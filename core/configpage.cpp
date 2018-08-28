/* This file is part of KDevelop
   Copyright 2018 Anton Anikin <anton@anikin.xyz>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING. If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "configpage.h"

#include "config.h"
#include "debug.h"

#include <KConfigDialogManager>

#include <QIcon>

namespace Valgrind
{

ConfigPage::ConfigPage(QString title, QWidget* parent)
    : LaunchConfigurationPage(parent)
    , m_title(title)
    , m_config(nullptr)
    , m_manager(nullptr)
{
}

ConfigPage::~ConfigPage() = default;

void ConfigPage::init(Config* config)
{
    Q_ASSERT(config);

    m_config.reset(config);
    m_manager.reset(new KConfigDialogManager(this, config));

    connect(m_manager.data(), &KConfigDialogManager::widgetModified, this, &ConfigPage::changed);
    connect(this, &ConfigPage::changed, this, &ConfigPage::check);

    check();
}

void ConfigPage::check()
{
}

QString ConfigPage::title() const
{
    return m_title;
}

QIcon ConfigPage::icon() const
{
    return QIcon();
}

void ConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    Q_ASSERT(m_config);
    Q_ASSERT(m_manager);

    m_config->setConfigGroup(cfg);
    m_config->load();

    m_manager->updateWidgets();
}

void ConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    Q_ASSERT(m_config);
    Q_ASSERT(m_manager);

    m_config->setConfigGroup(cfg);
    m_manager->updateSettings();
}

}
