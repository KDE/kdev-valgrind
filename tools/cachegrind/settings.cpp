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

namespace Cachegrind
{

Settings::Settings()
    : ISettings(QStringLiteral("Cachegrind"))

    , cacheSimulation(
        this,
        QStringLiteral("Cache Simulation"),
        QStringLiteral("cache-sim"),
        true)

    , branchSimulation(
        this,
        QStringLiteral("Branch Simulation"),
        QStringLiteral("branch-sim"),
        false)

    , cgAnnotateParameters(
        this,
        QStringLiteral("cg_annotate Parameters"),
        QStringLiteral(""),
        QStringLiteral(""))
{
}

Settings::~Settings()
{
}

QString Settings::cgAnnotateExecutablePath()
{
    return KDevelop::Path(GlobalSettings::cg_annotateExecutablePath()).toLocalFile();
}

}

}
