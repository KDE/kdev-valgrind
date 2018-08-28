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

#include "helgrind_config.h"

#include "helgrind_tool.h"

namespace Valgrind
{

HelgrindConfig::HelgrindConfig()
    : XmlConfig(HelgrindTool::self()->id())
{
    addCmdItemString(QStringLiteral("historyLevel"), m_historyLevel, QStringLiteral("full"), QStringLiteral("history-level"));

    addCmdItemInt(QStringLiteral("conflictCacheSize"), m_conflictCacheSize, 1000000, QStringLiteral("conflict-cache-size"));

    addCmdItemBool(QStringLiteral("trackLockorders"), m_trackLockorders, true, QStringLiteral("track-lockorders"));
    addCmdItemBool(QStringLiteral("checkStackRefs"), m_checkStackRefs, true, QStringLiteral("check-stack-refs"));
    addCmdItemBool(QStringLiteral("ignoreThreadCreation"), m_ignoreThreadCreation, false, QStringLiteral("ignore-thread-creation"));
    addCmdItemBool(QStringLiteral("freeIsWrite"), m_freeIsWrite, false, QStringLiteral("free-is-write"));
}

}
