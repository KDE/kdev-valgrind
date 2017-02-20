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

    connect(ui->extraParameters, &QLineEdit::textEdited, this, &ConfigPage::changed);
    connect(ui->limitErrors, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->maxStackframe, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ConfigPage::changed);
    connect(ui->numCallers, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
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
    return QIcon();
}

void ConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    QSignalBlocker blocker(this);
    Settings settings;
    settings.load(cfg);

    ui->extraParameters->setText(settings.extraParameters);
    ui->numCallers->setValue(settings.numCallers);
    ui->maxStackframe->setValue(settings.maxStackframe);
    ui->limitErrors->setChecked(settings.limitErrors);
}

void ConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    Settings settings;

    settings.extraParameters = ui->extraParameters->text();
    settings.numCallers = ui->numCallers->value();
    settings.maxStackframe = ui->maxStackframe->value();
    settings.limitErrors = ui->limitErrors->isChecked();

    settings.save(cfg);
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
