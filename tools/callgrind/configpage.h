/* This file is part of KDevelop
 * Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 * Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#pragma once

#include <interfaces/launchconfigurationpage.h>

namespace valgrind
{

namespace Ui
{

class CallgrindConfig;

}

class CallgrindConfigPage : public KDevelop::LaunchConfigurationPage
{
    Q_OBJECT
public:
    explicit CallgrindConfigPage(QWidget* parent = nullptr);

    QString title() const override;
    QIcon icon() const override;

    void loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject* project = nullptr) override;
    void saveToConfiguration(KConfigGroup cfg, KDevelop::IProject* project = nullptr) const override;

private:
    Ui::CallgrindConfig* ui;
};

class CallgrindConfigPageFactory : public KDevelop::LaunchConfigurationPageFactory
{
public:
    CallgrindConfigPageFactory();
    ~CallgrindConfigPageFactory() override;

    KDevelop::LaunchConfigurationPage* createWidget(QWidget* parent) override;
};

}
