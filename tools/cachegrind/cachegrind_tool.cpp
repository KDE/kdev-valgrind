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

#include "cachegrind_tool.h"

#include "cachegrind_configpage.h"
#include "cachegrind_job.h"

#include <KLocalizedString>

namespace Valgrind
{

CachegrindTool* CachegrindTool::m_self = nullptr;

CachegrindTool::CachegrindTool()
    : Tool(
        QStringLiteral("Cachegrind"),
        i18n("Cachegrind"),
        i18n("Cachegrind (Cache and Branch-Prediction Profiler)"),
        QStringLiteral("cachegrind"),
        QStringLiteral("cachegrind_tool"),
        true)
{
    m_self = this;
}

CachegrindTool::~CachegrindTool()
{
    m_self = nullptr;
}

CachegrindTool* CachegrindTool::self()
{
    return m_self ? m_self : new CachegrindTool;
}

Job* CachegrindTool::createJob(KDevelop::ILaunchConfiguration* launchConfig) const
{
    return new CachegrindJob(launchConfig);
}

KDevelop::LaunchConfigurationPageFactory* CachegrindTool::createConfigPageFactory() const
{
    return new CachegrindConfigPageFactory;
}

}
