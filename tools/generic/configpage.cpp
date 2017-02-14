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

namespace Valgrind
{

namespace Generic
{

ConfigPage::ConfigPage(QWidget* parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::ConfigPage();
    ui->setupUi(this);

    ui->currentTool->addItems(valgrindTools);

    connect(ui->extraParameters, &QLineEdit::textEdited, this, &ConfigPage::changed);
    connect(ui->limitErrors, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->maxStackSize, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ConfigPage::changed);
    connect(ui->stackDepth, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ConfigPage::changed);
    connect(ui->currentTool, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &ConfigPage::changed);
}

ConfigPage::~ConfigPage()
{
    delete ui;
}

QString ConfigPage::title() const
{
    return i18n("Valgrind generic settings");
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
    ui->stackDepth->setValue(settings.stackframeDepth());
    ui->maxStackSize->setValue(settings.maximumStackframeSize());
    ui->limitErrors->setChecked(settings.limitErrors());
    ui->currentTool->setCurrentIndex(settings.currentTool());
}

void ConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    Settings settings(cfg);

    settings.setExtraParameters(ui->extraParameters->text());
    settings.setStackframeDepth(ui->stackDepth->value());
    settings.setMaximumStackframeSize(ui->maxStackSize->value());
    settings.setLimitErrors(ui->limitErrors->isChecked());
    settings.setCurrentTool(ui->currentTool->currentIndex());
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
