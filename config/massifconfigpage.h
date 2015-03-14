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

#ifndef VALGRINDMASSIFCONFIGPAGE_H_
#define VALGRINDMASSIFCONFIGPAGE_H_

#include <QObject>
#include <interfaces/launchconfigurationpage.h>

namespace valgrind
{
namespace Ui
{
class MassifConfig;
}

class MassifConfigPage : public KDevelop::LaunchConfigurationPage
{
    Q_OBJECT
public:
    MassifConfigPage(QWidget * parent = 0);
    virtual void          loadFromConfiguration(const KConfigGroup&, KDevelop::IProject * = 0);
    virtual QIcon         icon(void) const;
    virtual void          saveToConfiguration(KConfigGroup, KDevelop::IProject * = 0) const;
    virtual QString       title(void) const;

private:
    Ui::MassifConfig  *ui;
};

class MassifConfigPageFactory : public KDevelop::LaunchConfigurationPageFactory
{
public:
    MassifConfigPageFactory(void);
    virtual ~MassifConfigPageFactory(void);
    virtual KDevelop::LaunchConfigurationPage* createWidget(QWidget * parent);
};

}

#endif /* VALGRINDMASSIFCONFIGPAGE_H_ */
