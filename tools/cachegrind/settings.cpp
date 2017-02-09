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

namespace Cachegrind
{

Settings::Settings(const KConfigGroup& config)
    : m_config(config)
{
}

QString Settings::extraParameters() const
{
    return m_config.readEntry(QStringLiteral("Cachegrind Extra Parameters"), QString{});
}

void Settings::setExtraParameters(const QString& parameters)
{
    m_config.writeEntry(QStringLiteral("Cachegrind Extra Parameters"), parameters);
}

bool Settings::cacheSimulation() const
{
    return m_config.readEntry(QStringLiteral("Cachegrind Cache Simulation"), true);
}

void Settings::setCacheSimulation(bool value)
{
    m_config.writeEntry(QStringLiteral("Cachegrind Cache Simulation"), value);
}

bool Settings::branchSimulation() const
{
    return m_config.readEntry(QStringLiteral("Cachegrind Branch Simulation"), false);
}

void Settings::setBranchSimulation(bool value)
{
    m_config.writeEntry(QStringLiteral("Cachegrind Branch Simulation"), value);
}

QString Settings::cgAnnotateExecutablePath()
{
    return KDevelop::Path(GlobalSettings::cg_annotateExecutablePath()).toLocalFile();
}

QString Settings::cgAnnotateParameters() const
{
    return m_config.readEntry(QStringLiteral("Cachegrind cg_annotate Parameters"), QString{});
}

void Settings::setCgAnnotateParameters(const QString& parameters)
{
    m_config.writeEntry(QStringLiteral("Cachegrind cg_annotate Parameters"), parameters);
}

}

}
