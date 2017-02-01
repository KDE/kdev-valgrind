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

namespace valgrind
{

namespace CallgrindSettings
{

QString extraParameters(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Callgrind Extra Parameters"), QString{});
}

void setExtraParameters(KConfigGroup& cfg, const QString& parameters)
{
    cfg.writeEntry(QStringLiteral("Callgrind Extra Parameters"), parameters);
}

bool cacheSimulation(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Callgrind Cache Simulation"), false);
}

void setCacheSimulation(KConfigGroup& cfg, bool value)
{
    cfg.writeEntry(QStringLiteral("Callgrind Cache Simulation"), value);
}

bool branchSimulation(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Callgrind Branch Simulation"), false);
}

void setBranchSimulation(KConfigGroup& cfg, bool value)
{
    cfg.writeEntry(QStringLiteral("Callgrind Branch Simulation"), value);
}

bool launchKCachegrind(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Callgrind Launch KCachegrind"), false);
}

void setLaunchKCachegrind(KConfigGroup& cfg, bool value)
{
    cfg.writeEntry(QStringLiteral("Callgrind Launch KCachegrind"), value);
}

}

}
