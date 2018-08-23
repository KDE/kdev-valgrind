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

#include "settings.h"

#include "tool.h"

namespace Valgrind
{

namespace Helgrind
{

Settings::Settings()
    : IXmlSettings(Tool::self()->id())

    , historyLevel(
        this,
        QStringLiteral("History Level"),
        QStringLiteral("history-level"),
        QStringLiteral("full"))

    , conflictCacheSize(
        this,
        QStringLiteral("Conflict Cache Size"),
        QStringLiteral("conflict-cache-size"),
        1000000)

    , trackLockorders(
        this,
        QStringLiteral("Track Lock Orders"),
        QStringLiteral("track-lockorders"),
        true)

    , checkStackRefs(
        this,
        QStringLiteral("Check Stack Refs"),
        QStringLiteral("check-stack-refs"),
        true)

    , ignoreThreadCreation(
        this,
        QStringLiteral("Ignore Thread Creation"),
        QStringLiteral("ignore-thread-creation"),
        false)

    , freeIsWrite(
        this,
        QStringLiteral("Free Is Write"),
        QStringLiteral("free-is-write"),
        false)
{
}

Settings::~Settings()
{
}

}

}
