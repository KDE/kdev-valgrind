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

#include "settings.h"

#include <KConfigGroup>

namespace valgrind
{

CallgrindConfigPage::CallgrindConfigPage(QWidget* parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::CallgrindConfig();
    ui->setupUi(this);

    connect(ui->extraParameters, &QLineEdit::textChanged, this, &CallgrindConfigPage::changed);
    connect(ui->cacheSimulation, &QCheckBox::toggled, this, &CallgrindConfigPage::changed);
    connect(ui->branchSimulation, &QCheckBox::toggled, this, &CallgrindConfigPage::changed);
    connect(ui->launchKCachegrind, &QCheckBox::toggled, this, &CallgrindConfigPage::changed);
    connect(ui->callgrindAnnotateParameters, &QLineEdit::textChanged, this, &CallgrindConfigPage::changed);
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
    CallgrindSettings settings(cfg);

    ui->extraParameters->setText(settings.extraParameters());
    ui->cacheSimulation->setChecked(settings.cacheSimulation());
    ui->branchSimulation->setChecked(settings.branchSimulation());
    ui->launchKCachegrind->setChecked(settings.launchKCachegrind());
    ui->callgrindAnnotateParameters->setText(settings.callgrindAnnotateParameters());
}


void CallgrindConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    CallgrindSettings settings(cfg);

    settings.setExtraParameters(ui->extraParameters->text());
    settings.setCacheSimulation(ui->cacheSimulation->isChecked());
    settings.setBranchSimulation(ui->branchSimulation->isChecked());
    settings.setLaunchKCachegrind(ui->launchKCachegrind->isChecked());
    settings.setCallgrindAnnotateParameters(ui->callgrindAnnotateParameters->text());
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
