/* This file is part of KDevelop
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@buffout.org>

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

#include <QObject>
#include <interfaces/launchconfigurationpage.h>

class ValgrindPlugin;

namespace Ui
{
  class ValgrindGenericConfig;
}

class	ValgrindGenericConfigPage : public KDevelop::LaunchConfigurationPage
{
  Q_OBJECT

public:

  ValgrindGenericConfigPage(ValgrindPlugin * plugin, QWidget * parent = 0);
  ~ValgrindGenericConfigPage();
  virtual void loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject * = 0);
  virtual KIcon icon() const;
  virtual void saveToConfiguration(KConfigGroup, KDevelop::IProject * = 0) const;
  virtual QString title() const;

signals:
  void newCurrentTool(QString newTool) const;

private:
  Ui::ValgrindGenericConfig *ui;
};

class ValgrindGenericConfigPageFactory : public KDevelop::LaunchConfigurationPageFactory
{
public:
  ValgrindGenericConfigPageFactory( ValgrindPlugin* );
  virtual ~ValgrindGenericConfigPageFactory();
  virtual KDevelop::LaunchConfigurationPage* createWidget(QWidget * parent);

private:
    ValgrindPlugin* m_plugin;
};


#endif /* VALGRINDGENERICCONFIGPAGE_H_ */
