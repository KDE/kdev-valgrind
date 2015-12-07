/* This file is part of KDevelop
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>

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

#ifndef VALGRINDGENERICCONFIGPAGE_H_
#define VALGRINDGENERICCONFIGPAGE_H_

#include <interfaces/launchconfigurationpage.h>

namespace valgrind
{
class Plugin;

namespace Ui
{
class GenericConfig;
}

class   GenericConfigPage : public KDevelop::LaunchConfigurationPage
{
    Q_OBJECT

public:

    GenericConfigPage(valgrind::Plugin * plugin, QWidget * parent = 0);
    ~GenericConfigPage();
    virtual void loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject * = 0);
    virtual QIcon icon() const;
    virtual void saveToConfiguration(KConfigGroup, KDevelop::IProject * = 0) const;
    virtual QString title() const;

signals:
    void newCurrentTool(QString newTool) const;

private:
    Ui::GenericConfig *ui;
};

class GenericConfigPageFactory : public KDevelop::LaunchConfigurationPageFactory
{
public:
    GenericConfigPageFactory(valgrind::Plugin*);
    virtual ~GenericConfigPageFactory();
    virtual KDevelop::LaunchConfigurationPage* createWidget(QWidget * parent);

private:
    valgrind::Plugin* m_plugin;
};
}

#endif /* VALGRINDGENERICCONFIGPAGE_H_ */
