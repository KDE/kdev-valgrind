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

CachegrindConfigPage::CachegrindConfigPage(QWidget *parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::CachegrindConfig();
    ui->setupUi(this);

    ui->messageWidget->setWordWrap(true);

    connect(ui->extraParameters, &QLineEdit::textChanged, this, &CachegrindConfigPage::changed);

    connect(ui->cacheSimulation, &QCheckBox::toggled, this, &CachegrindConfigPage::changed);
    connect(ui->branchSimulation, &QCheckBox::toggled, this, &CachegrindConfigPage::changed);

    connect(ui->cacheSimulation, &QCheckBox::toggled, this, &CachegrindConfigPage::check);
    connect(ui->branchSimulation, &QCheckBox::toggled, this, &CachegrindConfigPage::check);
}

QString CachegrindConfigPage::title() const
{
    return i18n("Cachegrind");
}

QIcon CachegrindConfigPage::icon() const
{
    return QIcon::fromTheme("fork");
}

void CachegrindConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    QSignalBlocker blocker(this);

    ui->extraParameters->setText(CachegrindSettings::extraParameters(cfg));
    ui->cacheSimulation->setChecked(CachegrindSettings::cacheSimulation(cfg));
    ui->branchSimulation->setChecked(CachegrindSettings::branchSimulation(cfg));

    check();
}

void CachegrindConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    CachegrindSettings::setExtraParameters(cfg, ui->extraParameters->text());
    CachegrindSettings::setCacheSimulation(cfg, ui->cacheSimulation->isChecked());
    CachegrindSettings::setBranchSimulation(cfg, ui->branchSimulation->isChecked());
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

CachegrindConfigPageFactory::~CachegrindConfigPageFactory()
{
}

KDevelop::LaunchConfigurationPage* CachegrindConfigPageFactory::createWidget(QWidget* parent)
{
    return new CachegrindConfigPage(parent);
}

}
