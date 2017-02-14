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

namespace Generic
{

Settings::Settings(const KConfigGroup& config)
    : m_config(config)
{
}

QString Settings::extraParameters() const
{
    return m_config.readEntry(QStringLiteral("Valgrind Extra Parameters"), QString{});
}

void Settings::setExtraParameters(const QString& parameters)
{
    m_config.writeEntry(QStringLiteral("Valgrind Extra Parameters"), parameters);
}

int Settings::stackframeDepth() const
{
    return m_config.readEntry(QStringLiteral("Valgrind Stackframe Depth"), 12);
}

void Settings::setStackframeDepth(int depth)
{
    m_config.writeEntry(QStringLiteral("Valgrind Stackframe Depth"), depth);
}

int Settings::maximumStackframeSize() const
{
    return m_config.readEntry(QStringLiteral("Valgrind Maximum Stackframe Size"), 2000000);
}

void Settings::setMaximumStackframeSize(int size)
{
    m_config.writeEntry(QStringLiteral("Valgrind Maximum Stackframe Size"), size);
}

bool Settings::limitErrors() const
{
    return m_config.readEntry(QStringLiteral("Valgrind Limit Errors"), true);
}

void Settings::setLimitErrors(bool value)
{
    m_config.writeEntry(QStringLiteral("Valgrind Limit Errors"), value);
}

QString Settings::valgrindExecutablePath()
{
    return KDevelop::Path(GlobalSettings::valgrindExecutablePath()).toLocalFile();
}

}

}
