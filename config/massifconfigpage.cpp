/* This file is part of KDevelop
 * Copyright 2011 Sebastien Rannou <mxs@buffout.org>
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

#include "massifconfigpage.h"

#include "ui_massifconfig.h"

namespace valgrind
{

MassifConfigPage::MassifConfigPage( QWidget *parent )
    : LaunchConfigurationPage( parent )
{
    ui = new Ui::MassifConfig();
    ui->setupUi( this );
    connect(ui->massifParameters, SIGNAL(textEdited(QString)), SIGNAL(changed()) );
    // TODO: finish the other ones (due on sunday)
}

void	MassifConfigPage::loadFromConfiguration( const KConfigGroup& cfg, KDevelop::IProject * )
{
    bool wasBlocked = signalsBlocked();
    blockSignals(true);

    ui->massifParameters->setText( cfg.readEntry("Massif Arguments", "") );

    blockSignals(wasBlocked);
}

KIcon	MassifConfigPage::icon( void ) const
{
    return KIcon( "fork" );
}

void	MassifConfigPage::saveToConfiguration( KConfigGroup cfg, KDevelop::IProject * ) const
{
  cfg.writeEntry( "Massif Arguments", ui->massifParameters->text() );
}

QString	MassifConfigPage::title( void ) const
{
    return i18n("Massif");
}

// The factory
MassifConfigPageFactory::MassifConfigPageFactory( void )
{}

MassifConfigPageFactory::~MassifConfigPageFactory( void )
{}

KDevelop::LaunchConfigurationPage* MassifConfigPageFactory::createWidget( QWidget *parent)
{
    return new valgrind::MassifConfigPage( parent );
}
}
