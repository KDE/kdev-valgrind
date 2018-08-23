/* This file is part of KDevelop
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2017 Anton Anikin <anton@anikin.xyz>

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

#include "cachegrind_configpage.h"
#include "ui_cachegrind_configpage.h"

#include "cachegrind_settings.h"
#include "cachegrind_tool.h"

#include <KConfigGroup>

namespace Valgrind
{

CachegrindConfigPage::CachegrindConfigPage(QWidget* parent)
    : ConfigPage(parent)
    , ui(new Ui::CachegrindConfigPage())
{
    ui->setupUi(this);

    connectToChanged(ui->extraParameters);

    connectToChanged(ui->cacheSimulation);
    connectToChanged(ui->branchSimulation);
    connectToChanged(ui->cgAnnotateParameters);

    connectToChanged(ui->cacheSimulation);
    connectToChanged(ui->branchSimulation);
}

CachegrindConfigPage::~CachegrindConfigPage() = default;

QString CachegrindConfigPage::title() const
{
    return CachegrindTool::self()->name();
}

QIcon CachegrindConfigPage::icon() const
{
    return QIcon();
}

void CachegrindConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    QSignalBlocker blocker(this);
    CachegrindSettings settings;
    settings.load(cfg);

    ui->extraParameters->setText(settings.extraParameters);
    ui->cacheSimulation->setChecked(settings.cacheSimulation);
    ui->branchSimulation->setChecked(settings.branchSimulation);
    ui->cgAnnotateParameters->setText(settings.cgAnnotateParameters);

    check();
}

void CachegrindConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    CachegrindSettings settings;

    settings.extraParameters = ui->extraParameters->text();
    settings.cacheSimulation = ui->cacheSimulation->isChecked();
    settings.branchSimulation = ui->branchSimulation->isChecked();
    settings.cgAnnotateParameters = ui->cgAnnotateParameters->text();

    settings.save(cfg);
}

void CachegrindConfigPage::check()
{
    if (!ui->cacheSimulation->isChecked() && !ui->branchSimulation->isChecked()) {
        ui->messageWidget->setVisible(true);
        return;
    }

    ui->messageWidget->setVisible(false);
}

CachegrindConfigPageFactory::CachegrindConfigPageFactory()
{
}

KDevelop::LaunchConfigurationPage* CachegrindConfigPageFactory::createWidget(QWidget* parent)
{
    return new CachegrindConfigPage(parent);
}

}
