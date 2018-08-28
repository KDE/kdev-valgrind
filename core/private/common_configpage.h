/* This file is part of KDevelop
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2017 Anton Anikin <anton@anikin.xyz>

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

#include "configpage.h"

namespace Valgrind
{

class CommonConfigPage : public ConfigPage
{
    Q_OBJECT

public:
    explicit CommonConfigPage(QWidget* parent = nullptr);
    ~CommonConfigPage() override = default;
};

class CommonConfigPageFactory : public KDevelop::LaunchConfigurationPageFactory
{
public:
    CommonConfigPageFactory();
    ~CommonConfigPageFactory() override = default;

    KDevelop::LaunchConfigurationPage* createWidget(QWidget* parent) override;
};

}
