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

#include "valgrindcachegrindconfigpage.h"

#include "ui_valgrindcachegrindconfig.h"
namespace valgrind
{

CachegrindConfigPage::CachegrindConfigPage( QWidget *parent )
    : LaunchConfigurationPage( parent )
{
    ui = new Ui::CachegrindConfig();
    ui->setupUi( this );
}

void	CachegrindConfigPage::loadFromConfiguration( const KConfigGroup&, KDevelop::IProject * )
{}

KIcon	CachegrindConfigPage::icon( void ) const
{
    return KIcon( "fork" );
}

void	CachegrindConfigPage::saveToConfiguration( KConfigGroup, KDevelop::IProject * ) const
{}

QString	CachegrindConfigPage::title( void ) const
{
    return i18n("Cachegrind");
}

// The factory
CachegrindConfigPageFactory::CachegrindConfigPageFactory( void )
{}

CachegrindConfigPageFactory::~CachegrindConfigPageFactory( void )
{}

KDevelop::LaunchConfigurationPage* CachegrindConfigPageFactory::createWidget( QWidget *parent)
{
    return new valgrind::CachegrindConfigPage( parent );
}
}
