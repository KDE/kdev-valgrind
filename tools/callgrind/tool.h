/* This file is part of KDevelop
   Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING. If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#pragma once

#include "itool.h"

namespace Valgrind
{

namespace Callgrind
{

class Tool : public ITool
{
public:
    ~Tool() override;

    static Tool* self();

    IJob* createJob(KDevelop::ILaunchConfiguration* launchConfig) const override;
    KDevelop::LaunchConfigurationPageFactory* createConfigPageFactory() const override;

protected:
    Tool();

    static Tool* m_self;
};

}

}