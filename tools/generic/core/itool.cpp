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

#include "itool.h"

#include "launcher.h"

namespace Valgrind
{

ITool::ITool(
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

ITool::~ITool()
{
}

QString ITool::name() const
{
    return m_name;
}

QString ITool::fullName() const
{
    return m_fullName;
}

QString ITool::valgrindToolName() const
{
    return m_valgrindToolName;
}

QString ITool::id() const
{
    return m_id;
}

QString ITool::menuActionName() const
{
    return m_menuActionName;
}

bool ITool::hasView() const
{
    return m_hasView;
}

KDevelop::ILauncher* ITool::createLauncher() const
{
    return new Launcher(this);
}

}
