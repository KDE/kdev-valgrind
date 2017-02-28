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

#include "settings.h"

#include "tool.h"

namespace Valgrind
{

namespace DRD
{

Settings::Settings()
    : IXmlSettings(Tool::self()->id())

    , checkStackVar(
        this,
        QStringLiteral("Check Stack Variables"),
        QStringLiteral("check-stack-var"),
        false)

    , firstRaceOnly(
        this,
        QStringLiteral("First Race Only"),
        QStringLiteral("first-race-only"),
        false)

    , freeIsWrite(
        this,
        QStringLiteral("Free is Write"),
        QStringLiteral("free-is-write"),
        false)

    , reportSignalUnlocked(
        this,
        QStringLiteral("Report Signal Unlocked"),
        QStringLiteral("report-signal-unlocked"),
        true)

    , segmentMerging(
        this,
        QStringLiteral("Segment Merging"),
        QStringLiteral("segment-merging"),
        true)

    , showConflSeg(
        this,
        QStringLiteral("Show Conflicting Segments"),
        QStringLiteral("show-confl-seg"),
        true)

    , showStackUsage(
        this,
        QStringLiteral("Show Stack Usage"),
        QStringLiteral("show-stack-usage"),
        false)

    , ignoreThreadCreation(
        this,
        QStringLiteral("Ignore Thread Creation"),
        QStringLiteral("ignore-thread-creation"),
        false)

    , traceAlloc(
        this,
        QStringLiteral("Trace Allocations"),
        QStringLiteral("trace-alloc"),
        false)

    , traceBarrier(
        this,
        QStringLiteral("Trace Barrier Activity"),
        QStringLiteral("trace-barrier"),
        false)

    , traceCond(
        this,
        QStringLiteral("Trace Condition Variable Activity"),
        QStringLiteral("trace-cond"),
        false)

    , traceForkJoin(
        this,
        QStringLiteral("Trace Thread Creation and Termination"),
        QStringLiteral("trace-fork-join"),
        false)

    , traceHb(
        this,
        QStringLiteral("Trace HB"),
        QStringLiteral("trace-hb"),
        false)

    , traceMutex(
        this,
        QStringLiteral("Trace Mutex Activity"),
        QStringLiteral("trace-mutex"),
        false)

    , traceRwlock(
        this,
        QStringLiteral("Trace ReaderWriter Lock Activity"),
        QStringLiteral("trace-rwlock"),
        false)

    , traceSemaphore(
        this,
        QStringLiteral("Trace Semaphore Activity"),
        QStringLiteral("trace-semaphore"),
        false)

    , joinListVol(
        this,
        QStringLiteral("Joined Threads Memory Access List Volume"),
        QStringLiteral("join-list-vol"),
        10)

    , segmentMergingInterval(
        this,
        QStringLiteral("Segment Merging Interval"),
        QStringLiteral("segment-merging-interval"),
        10)
{
}

Settings::~Settings()
{
}

}

}
