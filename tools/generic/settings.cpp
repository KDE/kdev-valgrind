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

namespace GenericSettings
{

QString extraParameters(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Valgrind Extra Parameters"), QString{});
}

void setExtraParameters(KConfigGroup& cfg, const QString& parameters)
{
    cfg.writeEntry(QStringLiteral("Valgrind Extra Parameters"), parameters);
}

int stackframeDepth(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Valgrind Stackframe Depth"), 12);
}

void setStackframeDepth(KConfigGroup& cfg, int depth)
{
    cfg.writeEntry(QStringLiteral("Valgrind Stackframe Depth"), depth);
}

int maximumStackframeSize(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Valgrind Maximum Stackframe Size"), 2000000);
}

void setMaximumStackframeSize(KConfigGroup& cfg, int size)
{
    cfg.writeEntry(QStringLiteral("Valgrind Maximum Stackframe Size"), size);
}

bool limitErrors(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Valgrind Limit Errors"), true);
}

void setLimitErrors(KConfigGroup& cfg, bool value)
{
    cfg.writeEntry(QStringLiteral("Valgrind Limit Errors"), value);
}

int currentTool(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Valgrind Current Tool"), 0);
}

void setCurrentTool(KConfigGroup& cfg, int tool)
{
    cfg.writeEntry(QStringLiteral("Valgrind Current Tool"), tool);
}

}

}
