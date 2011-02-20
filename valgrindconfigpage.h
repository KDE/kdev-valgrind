/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2009 Andreas Pakulat <apaku@gmx.de>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>

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

#ifndef VALGRINDCONFIGPAGE_H_
#define VALGRINDCONFIGPAGE_H_

#include <QObject>
#include <interfaces/launchconfigurationpage.h>


namespace Ui
{
class ValgrindConfig;
}

class ValgrindConfigPage : public KDevelop::LaunchConfigurationPage
{
Q_OBJECT
public:
    ValgrindConfigPage( QWidget* parent = 0 );
    virtual void loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject* = 0);
    virtual KIcon icon() const;
    virtual void saveToConfiguration(KConfigGroup, KDevelop::IProject* = 0 ) const;
    virtual QString title() const;
private:
    Ui::ValgrindConfig *ui;
};

class ValgrindConfigPageFactory : public KDevelop::LaunchConfigurationPageFactory
{
public:
    ValgrindConfigPageFactory();
    virtual KDevelop::LaunchConfigurationPage* createWidget(QWidget* parent);
};


#endif /* VALGRINDCONFIGPAGE_H_ */
