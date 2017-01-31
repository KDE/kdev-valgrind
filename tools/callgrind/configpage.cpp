/* This file is part of KDevelop
 * Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 * Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#include "configpage.h"
#include "ui_configpage.h"

#include <KConfigGroup>

namespace valgrind
{

CallgrindConfigPage::CallgrindConfigPage(QWidget* parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::CallgrindConfig();
    ui->setupUi(this);

    connect(ui->extraParameters, &QLineEdit::textEdited, this, &CallgrindConfigPage::changed);
    connect(ui->cacheSimulation, &QCheckBox::toggled, this, &CallgrindConfigPage::changed);
    connect(ui->branchSimulation, &QCheckBox::toggled, this, &CallgrindConfigPage::changed);
    connect(ui->launchKCachegrind, &QCheckBox::toggled, this, &CallgrindConfigPage::changed);
}

QString CallgrindConfigPage::title() const
{
    return i18n("Callgrind");
}

QIcon CallgrindConfigPage::icon() const
{
    return QIcon::fromTheme("fork");
}

void CallgrindConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    QSignalBlocker blocker(this);

    ui->extraParameters->setText(cfg.readEntry("Callgrind Extra Parameters", ""));
    ui->cacheSimulation->setChecked(cfg.readEntry("Callgrind Cache Simulation", false));
    ui->branchSimulation->setChecked(cfg.readEntry("Callgrind Branch Simulation", false));
    ui->launchKCachegrind->setChecked(cfg.readEntry("Callgrind Launch KCachegrind", false));
}


void CallgrindConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    cfg.writeEntry("Callgrind Extra Parameters", ui->extraParameters->text());
    cfg.writeEntry("Callgrind Cache Simulation", ui->cacheSimulation->isChecked());
    cfg.writeEntry("Callgrind Branch Simulation", ui->branchSimulation->isChecked());
    cfg.writeEntry("Callgrind Launch KCachegrind", ui->launchKCachegrind->isChecked());
}

CallgrindConfigPageFactory::CallgrindConfigPageFactory()
{
}

CallgrindConfigPageFactory::~CallgrindConfigPageFactory()
{
}

KDevelop::LaunchConfigurationPage* CallgrindConfigPageFactory::createWidget(QWidget* parent)
{
    return new CallgrindConfigPage(parent);
}

}
