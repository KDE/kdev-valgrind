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

namespace Valgrind
{

namespace Memcheck
{

Settings::Settings(const KConfigGroup& config)
    : m_config(config)
{
}

QString Settings::extraParameters() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Extra Parameters"), QString{});
}

void Settings::setExtraParameters(const QString& parameters)
{
    m_config.writeEntry(QStringLiteral("Memcheck Extra Parameters"), parameters);
}

int Settings::freeListSize() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Freelist Size"), 10000000);
}

void Settings::setFreeListSize(int size)
{
    m_config.writeEntry(QStringLiteral("Memcheck Freelist Size"), size);
}

bool Settings::showReachable() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Show Reachable"), true);
}

void Settings::setShowReachable(bool value)
{
    m_config.writeEntry(QStringLiteral("Memcheck Show Reachable"), value);
}

bool Settings::undefValueErrors() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Undef Value Errors"), true);
}

void Settings::setUndefValueErrors(bool value)
{
    m_config.writeEntry(QStringLiteral("Memcheck Undef Value Errors"), value);
}

bool Settings::showInstructionPointer() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Show Instruction Pointer"), false);
}

void Settings::setShowInstructionPointer(bool value)
{
    m_config.writeEntry(QStringLiteral("Memcheck Show Instruction Pointer"), value);
}

}

}
