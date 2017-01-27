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

class GenericConfigPage : public KDevelop::LaunchConfigurationPage
{
    Q_OBJECT

public:
    GenericConfigPage(QWidget * parent = nullptr);
    ~GenericConfigPage() override;
    void loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject * = nullptr) override;
    QIcon icon() const override;
    void saveToConfiguration(KConfigGroup, KDevelop::IProject * = nullptr) const override;
    QString title() const override;

signals:
    void newCurrentTool(const QString& newTool) const;

private:
    Ui::GenericConfig *ui;
};

class GenericConfigPageFactory : public KDevelop::LaunchConfigurationPageFactory
{
public:
    GenericConfigPageFactory();
    ~GenericConfigPageFactory() override;
    KDevelop::LaunchConfigurationPage* createWidget(QWidget * parent) override;
};

}

#endif
