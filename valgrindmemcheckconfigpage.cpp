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


#include "valgrindmemcheckconfigpage.h"

#include "ui_valgrindmemcheckconfig.h"


ValgrindMemcheckConfigPage::ValgrindMemcheckConfigPage(QWidget *parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::ValgrindMemcheckConfig();
    ui->setupUi(this);

    connect( ui->memcheckParameters, SIGNAL(textEdited(QString)), SIGNAL(changed()) );
    connect( ui->freeListSize, SIGNAL(valueChanged(int)), SIGNAL(changed()) );
    connect( ui->showReachable, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->trackOrigin, SIGNAL(toggled(bool)), SIGNAL(changed()) );
}

KIcon ValgrindMemcheckConfigPage::icon() const
{
    return KIcon("fork");
}

void ValgrindMemcheckConfigPage::loadFromConfiguration(const KConfigGroup &cfg, KDevelop::IProject *)
{
    bool wasBlocked = signalsBlocked();
    blockSignals(true);

    ui->memcheckParameters->setText( cfg.readEntry( "Memcheck Arguments", "" ) );
    ui->freeListSize->setValue( cfg.readEntry( "Freelist Size", 10000000 ) );
    ui->showReachable->setChecked( cfg.readEntry( "Show Reachable", false ) );
    ui->trackOrigin->setChecked( cfg.readEntry( "Track Origins", false ) );

    blockSignals(wasBlocked);
}

void ValgrindMemcheckConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject *) const
{
    cfg.writeEntry( "Memcheck Arguments", ui->memcheckParameters->text() );
    cfg.writeEntry( "Freelist Size", ui->freeListSize->value() );
    cfg.writeEntry( "Show Reachable", ui->showReachable->isChecked() );
    cfg.writeEntry( "Track Origins", ui->showReachable->isChecked() );
}

QString	ValgrindMemcheckConfigPage::title() const
{
  return i18n("Memcheck settings");
}

// The factory
ValgrindMemcheckConfigPageFactory::ValgrindMemcheckConfigPageFactory()
{}

ValgrindMemcheckConfigPageFactory::~ValgrindMemcheckConfigPageFactory()
{}

KDevelop::LaunchConfigurationPage* ValgrindMemcheckConfigPageFactory::createWidget(QWidget * parent)
{
  return new ValgrindMemcheckConfigPage(parent);
}
