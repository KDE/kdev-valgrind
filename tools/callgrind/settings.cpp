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

namespace valgrind
{

CallgrindSettings::CallgrindSettings(const KConfigGroup& config)
    : m_config(config)
{
}

QString CallgrindSettings::extraParameters() const
{
    return m_config.readEntry(QStringLiteral("Callgrind Extra Parameters"), QString{});
}

void CallgrindSettings::setExtraParameters(const QString& parameters)
{
    m_config.writeEntry(QStringLiteral("Callgrind Extra Parameters"), parameters);
}

bool CallgrindSettings::cacheSimulation() const
{
    return m_config.readEntry(QStringLiteral("Callgrind Cache Simulation"), false);
}

void CallgrindSettings::setCacheSimulation(bool value)
{
    m_config.writeEntry(QStringLiteral("Callgrind Cache Simulation"), value);
}

bool CallgrindSettings::branchSimulation() const
{
    return m_config.readEntry(QStringLiteral("Callgrind Branch Simulation"), false);
}

void CallgrindSettings::setBranchSimulation(bool value)
{
    m_config.writeEntry(QStringLiteral("Callgrind Branch Simulation"), value);
}

bool CallgrindSettings::launchKCachegrind() const
{
    return m_config.readEntry(QStringLiteral("Callgrind Launch KCachegrind"), false);
}

void CallgrindSettings::setLaunchKCachegrind(bool value)
{
    m_config.writeEntry(QStringLiteral("Callgrind Launch KCachegrind"), value);
}

QString CallgrindSettings::callgrind_annotateExecutablePath() const
{
    return KDevelop::Path(GlobalSettings::callgrind_annotateExecutablePath()).toLocalFile();
}

QString CallgrindSettings::kcachegrindExecutablePath() const
{
    return KDevelop::Path(GlobalSettings::kcachegrindExecutablePath()).toLocalFile();
}

}
