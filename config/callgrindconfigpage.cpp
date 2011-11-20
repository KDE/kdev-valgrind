/* This file is part of KDevelop
 * Copyright 2011 Sebastien Rannou <mxs@sbrk.org>

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

#include "callgrindconfigpage.h"

#include "ui_callgrindconfig.h"

namespace valgrind
{

CallgrindConfigPage::CallgrindConfigPage(QWidget *parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::CallgrindConfig();
    ui->setupUi(this);

    connect(ui->callgrindParameters, SIGNAL(textEdited(QString)), SIGNAL(changed()));
    connect(ui->launchKCachegrind, SIGNAL(toggled(bool)), SIGNAL(changed()));
    connect(ui->KCachegrindExecutable, SIGNAL(textEdited(QString)), SIGNAL(changed()));
}

void    CallgrindConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject *)
{
    bool wasBlocked = signalsBlocked();
    blockSignals(true);

    ui->callgrindParameters->setText(cfg.readEntry("Callgrind Arguments", ""));
    ui->launchKCachegrind->setChecked(cfg.readEntry("Launch KCachegrind", false));
    ui->KCachegrindExecutable->setText(cfg.readEntry("KCachegrindExecutable", "/usr/bin/kcachegrind"));

    blockSignals(wasBlocked);
}

KIcon   CallgrindConfigPage::icon(void) const
{
    return KIcon("fork");
}

void    CallgrindConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject *) const
{
    cfg.writeEntry("Cachegrind Arguments", ui->callgrindParameters->text());
    cfg.writeEntry("Launch KCachegrind", ui->launchKCachegrind->isChecked());
    cfg.writeEntry("KCachegrindExecutable", ui->KCachegrindExecutable->text());
}

QString CallgrindConfigPage::title(void) const
{
    return i18n("Callgrind");
}

// The factory
CallgrindConfigPageFactory::CallgrindConfigPageFactory(void)
{}

CallgrindConfigPageFactory::~CallgrindConfigPageFactory(void)
{}

KDevelop::LaunchConfigurationPage* CallgrindConfigPageFactory::createWidget(QWidget *parent)
{
    return new valgrind::CallgrindConfigPage(parent);
}
}
