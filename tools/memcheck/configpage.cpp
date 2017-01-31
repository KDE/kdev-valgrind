/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>

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

MemcheckConfigPage::MemcheckConfigPage(QWidget *parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::MemcheckConfig();
    ui->setupUi(this);

    connect(ui->extraParameters, &QLineEdit::textEdited, this, &MemcheckConfigPage::changed);
    connect(ui->freeListSize, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MemcheckConfigPage::changed);

    connect(ui->showReachable, &QCheckBox::toggled, this, &MemcheckConfigPage::changed);
    connect(ui->undefValueErrors, &QCheckBox::toggled, this, &MemcheckConfigPage::changed);
    connect(ui->showInstructionPointer, &QCheckBox::toggled, this, &MemcheckConfigPage::changed);
}

QIcon MemcheckConfigPage::icon() const
{
    return QIcon::fromTheme("fork");
}

void MemcheckConfigPage::loadFromConfiguration(const KConfigGroup &cfg, KDevelop::IProject *)
{
    QSignalBlocker blocker(this);

    ui->extraParameters->setText(cfg.readEntry("Memcheck Extra Parameters", ""));
    ui->freeListSize->setValue(cfg.readEntry("Memcheck Freelist Size", 10000000));

    ui->showReachable->setChecked(cfg.readEntry("Memcheck Show Reachable", true));
    ui->undefValueErrors->setChecked(cfg.readEntry("Memcheck Undef Value Errors", true));
    ui->showInstructionPointer->setChecked(cfg.readEntry("Memcheck Show Instruction Pointer", false));
}

void MemcheckConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject *) const
{
    cfg.writeEntry("Memcheck Extra Parameters", ui->extraParameters->text());
    cfg.writeEntry("Memcheck Freelist Size", ui->freeListSize->value());

    cfg.writeEntry("Memcheck Show Reachable", ui->showReachable->isChecked());
    cfg.writeEntry("Memcheck Undef Value Errors", ui->undefValueErrors->isChecked());
    cfg.writeEntry("Memcheck Show Instruction Pointer", ui->showInstructionPointer->isChecked());
}

QString MemcheckConfigPage::title() const
{
    return i18n("Memcheck");
}

MemcheckConfigPageFactory::MemcheckConfigPageFactory()
{
}

MemcheckConfigPageFactory::~MemcheckConfigPageFactory()
{
}

KDevelop::LaunchConfigurationPage* MemcheckConfigPageFactory::createWidget(QWidget * parent)
{
    return new MemcheckConfigPage(parent);
}

}
