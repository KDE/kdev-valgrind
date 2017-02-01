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

#include "config/tools.h"

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

    ui->extraParameters->setText(cfg.readEntry("Valgrind Extra Parameters", ""));
    ui->stackDepth->setValue(cfg.readEntry("Valgrind Stackframe Depth", 12));
    ui->maxStackSize->setValue(cfg.readEntry("Valgrind Maximum Stackframe Size", 2000000));
    ui->limitErrors->setChecked(cfg.readEntry("Valgrind Limit Errors", true));
    ui->currentTool->setCurrentIndex(cfg.readEntry("Valgrind Current Tool", 0));
}

void GenericConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    cfg.writeEntry("Valgrind Extra Parameters", ui->extraParameters->text());
    cfg.writeEntry("Valgrind Stackframe Depth", ui->stackDepth->value());
    cfg.writeEntry("Valgrind Maximum Stackframe Size", ui->maxStackSize->value());
    cfg.writeEntry("Valgrind Limit Errors", ui->limitErrors->isChecked());
    cfg.writeEntry("Valgrind Current Tool", ui->currentTool->currentIndex());
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