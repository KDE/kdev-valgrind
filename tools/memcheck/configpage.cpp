/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
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

MemcheckConfigPage::MemcheckConfigPage(QWidget* parent)
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

QString MemcheckConfigPage::title() const
{
    return i18n("Memcheck");
}

QIcon MemcheckConfigPage::icon() const
{
    return QIcon::fromTheme("fork");
}

void MemcheckConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    QSignalBlocker blocker(this);
    MemcheckSettings settings(cfg);

    ui->extraParameters->setText(settings.extraParameters());
    ui->freeListSize->setValue(settings.freeListSize());

    ui->showReachable->setChecked(settings.showReachable());
    ui->undefValueErrors->setChecked(settings.undefValueErrors());
    ui->showInstructionPointer->setChecked(settings.showInstructionPointer());
}

void MemcheckConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    MemcheckSettings settings(cfg);

    settings.setExtraParameters(ui->extraParameters->text());
    settings.setFreeListSize(ui->freeListSize->value());

    settings.setShowReachable(ui->showReachable->isChecked());
    settings.setUndefValueErrors(ui->undefValueErrors->isChecked());
    settings.setShowInstructionPointer(ui->showInstructionPointer->isChecked());
}

MemcheckConfigPageFactory::MemcheckConfigPageFactory()
{
}

MemcheckConfigPageFactory::~MemcheckConfigPageFactory()
{
}

KDevelop::LaunchConfigurationPage* MemcheckConfigPageFactory::createWidget(QWidget* parent)
{
    return new MemcheckConfigPage(parent);
}

}
