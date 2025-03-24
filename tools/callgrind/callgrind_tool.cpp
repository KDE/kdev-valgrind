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

#include "callgrind_tool.h"

#include "callgrind_configpage.h"
#include "callgrind_job.h"

#include <KLocalizedString>

namespace Valgrind
{

CallgrindTool* CallgrindTool::m_self = nullptr;

CallgrindTool::CallgrindTool()
    : Tool(
        QStringLiteral("Callgrind"),
        i18n("Callgrind"),
        i18n("Callgrind (Call-Graph Generating Cache and Branch Prediction Profiler)"),
        QStringLiteral("callgrind"),
        QStringLiteral("callgrind_tool"),
        true)
{
    m_self = this;
}

CallgrindTool::~CallgrindTool()
{
    m_self = nullptr;
}

CallgrindTool* CallgrindTool::self()
{
    return m_self ? m_self : new CallgrindTool;
}

Job* CallgrindTool::createJob(const LaunchInfo& launchInfo) const
{
    return new CallgrindJob(launchInfo);
}

KDevelop::LaunchConfigurationPageFactory* CallgrindTool::createConfigPageFactory() const
{
    return new CallgrindConfigPageFactory;
}

}
