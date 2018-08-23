/* This file is part of KDevelop
   Copyright 2017 Anton Anikin <anton@anikin.xyz>

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

#include "tool.h"

#include "configpage.h"
#include "job.h"

#include <klocalizedstring.h>

namespace Valgrind
{

namespace Memcheck
{

Tool* Tool::m_self = nullptr;

Tool::Tool()
    : ITool(
        QStringLiteral("Memcheck"),
        i18n("Memcheck"),
        i18n("Memcheck (Memory Error Detector)"),
        QStringLiteral("memcheck"),
        QStringLiteral("memcheck_tool"),
        false)
{
    m_self = this;
}

Tool::~Tool()
{
    m_self = nullptr;
}

Tool* Tool::self()
{
    return m_self ? m_self : new Tool;
}

IJob* Tool::createJob(KDevelop::ILaunchConfiguration* launchConfig) const
{
    return new Job(launchConfig);
}

KDevelop::LaunchConfigurationPageFactory* Tool::createConfigPageFactory() const
{
    return new ConfigPageFactory;
}

}

}
