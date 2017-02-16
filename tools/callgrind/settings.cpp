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

#include "globalsettings.h"

namespace Valgrind
{

namespace Callgrind
{

Settings::Settings()
    : extraParameters(
        this,
        QStringLiteral("Callgrind Extra Parameters"),
        QStringLiteral(""),
        QStringLiteral(""))

    , callgrindAnnotateParameters(
        this,
        QStringLiteral("Callgrind callgrind_annotate Parameters"),
        QStringLiteral(""),
        QStringLiteral(""))

    , cacheSimulation(
        this,
        QStringLiteral("Callgrind Cache Simulation"),
        QStringLiteral("cache-sim"),
        false)

    , branchSimulation(
        this,
        QStringLiteral("Callgrind Branch Simulation"),
        QStringLiteral("branch-sim"),
        false)

    , launchKCachegrind(
        this,
        QStringLiteral("Callgrind Launch KCachegrind"),
        QStringLiteral(""),
        false)
{
}

Settings::~Settings()
{
}

QString Settings::callgrindAnnotateExecutablePath()
{
    return KDevelop::Path(GlobalSettings::callgrind_annotateExecutablePath()).toLocalFile();
}

QString Settings::kcachegrindExecutablePath()
{
    return KDevelop::Path(GlobalSettings::kcachegrindExecutablePath()).toLocalFile();
}

}

}
