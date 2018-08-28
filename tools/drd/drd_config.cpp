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

#include "drd_config.h"

#include "drd_tool.h"

namespace Valgrind
{

DrdConfig::DrdConfig()
    : XmlConfig(DrdTool::self()->id())
{
    addCmdItemBool(QStringLiteral("checkStackVar"), m_checkStackVar, false, QStringLiteral("check-stack-var"));
    addCmdItemBool(QStringLiteral("firstRaceOnly"), m_firstRaceOnly, false, QStringLiteral("first-race-only"));
    addCmdItemBool(QStringLiteral("freeIsWrite"), m_freeIsWrite, false, QStringLiteral("free-is-write"));
    addCmdItemBool(QStringLiteral("reportSignalUnlocked"), m_reportSignalUnlocked, true, QStringLiteral("report-signal-unlocked"));
    addCmdItemBool(QStringLiteral("segmentMerging"), m_segmentMerging, true, QStringLiteral("segment-merging"));
    addCmdItemBool(QStringLiteral("showConflSeg"), m_showConflSeg, true, QStringLiteral("show-confl-seg"));
    addCmdItemBool(QStringLiteral("showStackUsage"), m_showStackUsage, false, QStringLiteral("show-stack-usage"));
    addCmdItemBool(QStringLiteral("ignoreThreadCreation"), m_ignoreThreadCreation, false, QStringLiteral("ignore-thread-creation"));

    addCmdItemBool(QStringLiteral("traceAlloc"), m_traceAlloc, false, QStringLiteral("trace-alloc"));
    addCmdItemBool(QStringLiteral("traceBarrier"), m_traceBarrier, false, QStringLiteral("trace-barrier"));
    addCmdItemBool(QStringLiteral("traceCond"), m_traceCond, false, QStringLiteral("trace-cond"));
    addCmdItemBool(QStringLiteral("traceForkJoin"), m_traceForkJoin, false, QStringLiteral("trace-fork-join"));
    addCmdItemBool(QStringLiteral("traceHb"), m_traceHb, false, QStringLiteral("trace-hb"));
    addCmdItemBool(QStringLiteral("traceMutex"), m_traceMutex, false, QStringLiteral("trace-mutex"));
    addCmdItemBool(QStringLiteral("traceRwlock"), m_traceRwlock, false, QStringLiteral("trace-rwlock"));
    addCmdItemBool(QStringLiteral("traceSemaphore"), m_traceSemaphore, false, QStringLiteral("trace-semaphore"));

    addCmdItemInt(QStringLiteral("joinListVol"), m_joinListVol, 10, QStringLiteral("join-list-vol"));
    addCmdItemInt(QStringLiteral("segmentMergingInterval"), m_segmentMergingInterval, 10, QStringLiteral("segment-merging-interval"));
}

}
