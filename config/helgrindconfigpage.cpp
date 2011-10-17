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

#include "helgrindconfigpage.h"

#include "ui_helgrindconfig.h"

namespace valgrind
{

HelgrindConfigPage::HelgrindConfigPage( QWidget *parent )
    : LaunchConfigurationPage( parent )
{
    ui = new Ui::HelgrindConfig();
    ui->setupUi( this );

    QStringList events;

    events << "None (locks only)";
    events << "Threads";
    events << "All";

    ui->extraSynchronizationEvents->addItems( events );
    connect( ui->extraSynchronizationEvents, SIGNAL(currentIndexChanged(int)), SIGNAL(changed()) );
}

void	HelgrindConfigPage::loadFromConfiguration( const KConfigGroup& cfg, KDevelop::IProject * )
{
    bool wasBlocked = signalsBlocked();
    blockSignals(true);

    int extra_events_index;
    QString extra_events;

    extra_events = cfg.readEntry( "Extra Synchronization Events", "None (locks only)" );
    extra_events_index = ui->extraSynchronizationEvents->findText( extra_events );
    ui->extraSynchronizationEvents->setCurrentIndex( extra_events_index );

    blockSignals(wasBlocked);
}

KIcon	HelgrindConfigPage::icon( void ) const
{
    return KIcon( "fork" );
}

void	HelgrindConfigPage::saveToConfiguration( KConfigGroup cfg, KDevelop::IProject * ) const
{
    cfg.writeEntry( "Extra Synchronization Events", ui->extraSynchronizationEvents->currentText() );
}

QString	HelgrindConfigPage::title( void ) const
{
    return i18n("Helgrind");
}

// The factory
HelgrindConfigPageFactory::HelgrindConfigPageFactory( void )
{}

HelgrindConfigPageFactory::~HelgrindConfigPageFactory( void )
{}

KDevelop::LaunchConfigurationPage* HelgrindConfigPageFactory::createWidget( QWidget *parent)
{
    return new valgrind::HelgrindConfigPage( parent );
}
}
