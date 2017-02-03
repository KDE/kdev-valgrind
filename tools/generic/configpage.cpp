/* This file is part of KDevelop
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

#include <kconfiggroup.h>

namespace valgrind
{

GenericConfigPage::GenericConfigPage(QWidget* parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::GenericConfig();
    ui->setupUi(this);

    ui->currentTool->addItems(valgrindTools);

    connect(ui->extraParameters, &QLineEdit::textEdited, this, &GenericConfigPage::changed);
    connect(ui->limitErrors, &QCheckBox::toggled, this, &GenericConfigPage::changed);
    connect(ui->maxStackSize, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &GenericConfigPage::changed);
    connect(ui->stackDepth, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &GenericConfigPage::changed);
    connect(ui->currentTool, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &GenericConfigPage::changed);
}

GenericConfigPage::~GenericConfigPage()
{
}

QString GenericConfigPage::title() const
{
    return i18n("Valgrind global settings");
}

QIcon GenericConfigPage::icon() const
{
    return QIcon::fromTheme("fork");
}

void GenericConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    QSignalBlocker blocker(this);
    GenericSettings settings(cfg);

    ui->extraParameters->setText(settings.extraParameters());
    ui->stackDepth->setValue(settings.stackframeDepth());
    ui->maxStackSize->setValue(settings.maximumStackframeSize());
    ui->limitErrors->setChecked(settings.limitErrors());
    ui->currentTool->setCurrentIndex(settings.currentTool());
}

void GenericConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    GenericSettings settings(cfg);

    settings.setExtraParameters(ui->extraParameters->text());
    settings.setStackframeDepth(ui->stackDepth->value());
    settings.setMaximumStackframeSize(ui->maxStackSize->value());
    settings.setLimitErrors(ui->limitErrors->isChecked());
    settings.setCurrentTool(ui->currentTool->currentIndex());
}

GenericConfigPageFactory::GenericConfigPageFactory()
{
}

GenericConfigPageFactory::~GenericConfigPageFactory()
{
}

KDevelop::LaunchConfigurationPage* GenericConfigPageFactory::createWidget(QWidget* parent)
{
    return new GenericConfigPage(parent);
}

}
