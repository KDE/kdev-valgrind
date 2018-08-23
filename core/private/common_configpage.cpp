/* This file is part of KDevelop
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

#include "common_configpage.h"
#include "ui_common_configpage.h"

#include "common_settings.h"

#include <KConfigGroup>

namespace Valgrind
{

CommonConfigPage::CommonConfigPage(QWidget* parent)
    : ConfigPage(parent)
    , ui(new Ui::CommonConfigPage())
{
    ui->setupUi(this);

    connectToChanged(ui->extraParameters);
    connectToChanged(ui->limitErrors);
    connectToChanged(ui->maxStackframe);
    connectToChanged(ui->numCallers);

    ui->numCallersLabel->setToolTip(ui->numCallers->toolTip());
    ui->maxStackframeLabel->setToolTip(ui->maxStackframe->toolTip());
}

CommonConfigPage::~CommonConfigPage() = default;

QString CommonConfigPage::title() const
{
    return i18n("Valgrind Generic Settings");
}

QIcon CommonConfigPage::icon() const
{
    return QIcon();
}

void CommonConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    QSignalBlocker blocker(this);
    CommonSettings settings;
    settings.load(cfg);

    ui->extraParameters->setText(settings.extraParameters);
    ui->numCallers->setValue(settings.numCallers);
    ui->maxStackframe->setValue(settings.maxStackframe);
    ui->limitErrors->setChecked(settings.limitErrors);
}

void CommonConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    CommonSettings settings;

    settings.extraParameters = ui->extraParameters->text();
    settings.numCallers = ui->numCallers->value();
    settings.maxStackframe = ui->maxStackframe->value();
    settings.limitErrors = ui->limitErrors->isChecked();

    settings.save(cfg);
}

CommonConfigPageFactory::CommonConfigPageFactory()
{
}

KDevelop::LaunchConfigurationPage* CommonConfigPageFactory::createWidget(QWidget* parent)
{
    return new CommonConfigPage(parent);
}

}
