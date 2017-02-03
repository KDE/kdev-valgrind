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

GenericSettings::GenericSettings(const KConfigGroup& config)
    : m_config(config)
{
}

QString GenericSettings::extraParameters() const
{
    return m_config.readEntry(QStringLiteral("Valgrind Extra Parameters"), QString{});
}

void GenericSettings::setExtraParameters(const QString& parameters)
{
    m_config.writeEntry(QStringLiteral("Valgrind Extra Parameters"), parameters);
}

int GenericSettings::stackframeDepth() const
{
    return m_config.readEntry(QStringLiteral("Valgrind Stackframe Depth"), 12);
}

void GenericSettings::setStackframeDepth(int depth)
{
    m_config.writeEntry(QStringLiteral("Valgrind Stackframe Depth"), depth);
}

int GenericSettings::maximumStackframeSize() const
{
    return m_config.readEntry(QStringLiteral("Valgrind Maximum Stackframe Size"), 2000000);
}

void GenericSettings::setMaximumStackframeSize(int size)
{
    m_config.writeEntry(QStringLiteral("Valgrind Maximum Stackframe Size"), size);
}

bool GenericSettings::limitErrors() const
{
    return m_config.readEntry(QStringLiteral("Valgrind Limit Errors"), true);
}

void GenericSettings::setLimitErrors(bool value)
{
    m_config.writeEntry(QStringLiteral("Valgrind Limit Errors"), value);
}

int GenericSettings::currentTool() const
{
    return m_config.readEntry(QStringLiteral("Valgrind Current Tool"), 0);
}

void GenericSettings::setCurrentTool(int tool)
{
    m_config.writeEntry(QStringLiteral("Valgrind Current Tool"), tool);
}

QString GenericSettings::valgrindExecutablePath() const
{
    return KDevelop::Path(GlobalSettings::valgrindExecutablePath()).toLocalFile();
}

}
