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

#include "tool.h"

#include "private/launcher.h"

namespace Valgrind
{

Tool::Tool(
    const QString& id,
    const QString& name,
    const QString& fullName,
    const QString& valgrindToolName,
    const QString& menuActionName,
    bool hasView)

    : m_id(id)
    , m_name(name)
    , m_fullName(fullName)
    , m_valgrindToolName(valgrindToolName)
    , m_menuActionName(menuActionName)
    , m_hasView(hasView)
{
}

Tool::~Tool()
{
}

QString Tool::name() const
{
    return m_name;
}

QString Tool::fullName() const
{
    return m_fullName;
}

QString Tool::valgrindToolName() const
{
    return m_valgrindToolName;
}

QString Tool::id() const
{
    return m_id;
}

QString Tool::menuActionName() const
{
    return m_menuActionName;
}

bool Tool::hasView() const
{
    return m_hasView;
}

KDevelop::ILauncher* Tool::createLauncher() const
{
    return new Launcher(this);
}

}
