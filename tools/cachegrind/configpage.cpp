/* This file is part of KDevelop
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

namespace Cachegrind
{

ConfigPage::ConfigPage(QWidget *parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::ConfigPage();
    ui->setupUi(this);

    connect(ui->extraParameters, &QLineEdit::textChanged, this, &ConfigPage::changed);

    connect(ui->cacheSimulation, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->branchSimulation, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->cgAnnotateParameters, &QLineEdit::textChanged, this, &ConfigPage::changed);

    connect(ui->cacheSimulation, &QCheckBox::toggled, this, &ConfigPage::check);
    connect(ui->branchSimulation, &QCheckBox::toggled, this, &ConfigPage::check);
}

QString ConfigPage::title() const
{
    return i18n("Cachegrind");
}

QIcon ConfigPage::icon() const
{
    return QIcon::fromTheme("fork");
}

void ConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    QSignalBlocker blocker(this);
    Settings settings(cfg);

    ui->extraParameters->setText(settings.extraParameters());
    ui->cacheSimulation->setChecked(settings.cacheSimulation());
    ui->branchSimulation->setChecked(settings.branchSimulation());
    ui->cgAnnotateParameters->setText(settings.cgAnnotateParameters());

    check();
}

void ConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    Settings settings(cfg);

    settings.setExtraParameters(ui->extraParameters->text());
    settings.setCacheSimulation(ui->cacheSimulation->isChecked());
    settings.setBranchSimulation(ui->branchSimulation->isChecked());
    settings.setCgAnnotateParameters(ui->cgAnnotateParameters->text());
}

void ConfigPage::check()
{
    if (!ui->cacheSimulation->isChecked() && !ui->branchSimulation->isChecked()) {
        ui->messageWidget->setVisible(true);
        return;
    }

    ui->messageWidget->setVisible(false);
}

ConfigPageFactory::ConfigPageFactory()
{
}

ConfigPageFactory::~ConfigPageFactory()
{
}

KDevelop::LaunchConfigurationPage* ConfigPageFactory::createWidget(QWidget* parent)
{
    return new ConfigPage(parent);
}

}

}
