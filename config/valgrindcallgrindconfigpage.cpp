/* This file is part of KDevelop
 * Copyright 2011 Sebastien Rannou <mxs@buffout.org>

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

#include "valgrindcallgrindconfigpage.h"

#include "ui_valgrindcallgrindconfig.h"

ValgrindCallgrindConfigPage::ValgrindCallgrindConfigPage(QWidget *parent)
    : LaunchConfigurationPage( parent )
{
    ui = new Ui::ValgrindCallgrindConfig();
    ui->setupUi( this );

    connect( ui->separateThreadReporting, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->fullCacheSimulation, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->simulateHardwarePrefetcher, SIGNAL(toggled(bool)), SIGNAL(changed()) );
}

void	ValgrindCallgrindConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject *)
{
    bool wasBlocked = signalsBlocked();
    blockSignals(true);

    ui->separateThreadReporting->setChecked( cfg.readEntry( "Separate Thread Reporting", false) );
    ui->fullCacheSimulation->setChecked( cfg.readEntry( "Full Cache Simulation", false) );
    ui->simulateHardwarePrefetcher->setChecked( cfg.readEntry( "Simulate Hardware Prefetcher", false) );

    blockSignals(wasBlocked);
}

KIcon	ValgrindCallgrindConfigPage::icon( void ) const
{
    return KIcon( "fork" );
}

void	ValgrindCallgrindConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject *) const
{
    cfg.writeEntry( "Separate Thread Reporting", ui->separateThreadReporting->isChecked() );
    cfg.writeEntry( "Full Cache Simulation", ui->fullCacheSimulation->isChecked() );
    cfg.writeEntry( "Simulate Hardware Prefetcher", ui->simulateHardwarePrefetcher->isChecked() );
}

QString	ValgrindCallgrindConfigPage::title(void) const
{
    return i18n("Callgrind");
}

// The factory
ValgrindCallgrindConfigPageFactory::ValgrindCallgrindConfigPageFactory(void)
{}

ValgrindCallgrindConfigPageFactory::~ValgrindCallgrindConfigPageFactory(void)
{}

KDevelop::LaunchConfigurationPage* ValgrindCallgrindConfigPageFactory::createWidget(QWidget *parent)
{
    return new ValgrindCallgrindConfigPage( parent );
}
