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

ValgrindCachegrindConfigPage::ValgrindCachegrindConfigPage( QWidget *parent )
  : LaunchConfigurationPage( parent )  
{
  ui = new Ui::ValgrindCachegrindConfig();
  ui->setupUi( this );
}

void	ValgrindCachegrindConfigPage::loadFromConfiguration( const KConfigGroup&, KDevelop::IProject * )
{}

KIcon	ValgrindCachegrindConfigPage::icon( void ) const
{
  return KIcon( "fork" );
}

void	ValgrindCachegrindConfigPage::saveToConfiguration( KConfigGroup, KDevelop::IProject * ) const
{}

QString	ValgrindCachegrindConfigPage::title( void ) const
{
  return i18n("Cachegrind settings");
}

// The factory
ValgrindCachegrindConfigPageFactory::ValgrindCachegrindConfigPageFactory( void )
{}

ValgrindCachegrindConfigPageFactory::~ValgrindCachegrindConfigPageFactory( void )
{}

KDevelop::LaunchConfigurationPage* ValgrindCachegrindConfigPageFactory::createWidget( QWidget *parent)
{
  return new ValgrindCachegrindConfigPage( parent );
}
