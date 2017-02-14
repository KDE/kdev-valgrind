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

namespace Helgrind
{

Settings::Settings(const KConfigGroup& config)
    : m_config(config)
{
}

QString Settings::extraParameters() const
{
    return m_config.readEntry(QStringLiteral("Helgrind Extra Parameters"), QString{});
}

void Settings::setExtraParameters(const QString& parameters)
{
    m_config.writeEntry(QStringLiteral("Helgrind Extra Parameters"), parameters);
}

int Settings::conflictCacheSize() const
{
    return m_config.readEntry(QStringLiteral("Helgrind Conflict Cache Size"), 1000000);
}

void Settings::setConflictCacheSize(int size)
{
    m_config.writeEntry(QStringLiteral("Helgrind Conflict Cache Size"), size);
}

QString Settings::historyLevel() const
{
    return m_config.readEntry(QStringLiteral("Helgrind History Level"), QStringLiteral("full"));
}

void Settings::setHistoryLevel(const QString& level)
{
    m_config.writeEntry(QStringLiteral("Helgrind History Level"), level);
}

bool Settings::trackLockOrders() const
{
    return m_config.readEntry(QStringLiteral("Helgrind Track Lock Orders"), true);
}

void Settings::setTrackLockOrders(bool value)
{
    m_config.writeEntry(QStringLiteral("Helgrind Track Lock Orders"), value);
}

bool Settings::checkStackRefs() const
{
    return m_config.readEntry(QStringLiteral("Helgrind Check Stack Refs"), true);
}

void Settings::setCheckStackRefs(bool value)
{
    m_config.writeEntry(QStringLiteral("Helgrind Check Stack Refs"), value);
}

bool Settings::ignoreThreadCreation() const
{
    return m_config.readEntry(QStringLiteral("Helgrind Ignore Thread Creation"), false);
}

void Settings::setIgnoreThreadCreation(bool value)
{
    m_config.writeEntry(QStringLiteral("Helgrind Ignore Thread Creation"), value);
}

}

}
