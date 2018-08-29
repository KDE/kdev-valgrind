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

#include "memcheck_config.h"

#include "memcheck_tool.h"

namespace Valgrind
{

MemcheckConfig::MemcheckConfig()
    : XmlConfig(MemcheckTool::self()->id())
{
    addCmdItemString(QStringLiteral("leakResolution"), m_leakResolution, QStringLiteral("high"), QStringLiteral("leak-resolution"));
    addCmdItemString(QStringLiteral("showLeakKinds"), m_showLeakKinds, QStringLiteral("definite,possible"), QStringLiteral("show-leak-kinds"));
    addCmdItemString(QStringLiteral("leakCheckHeuristics"), m_leakCheckHeuristics, QStringLiteral("all"), QStringLiteral("leak-check-heuristics"));
    addCmdItemString(QStringLiteral("keepStacktraces"), m_keepStacktraces, QStringLiteral("alloc-and-free"), QStringLiteral("keep-stacktraces"));

    auto itemInt = addCmdItemInt(QStringLiteral("freelistVol"), m_freelistVol, 20000000, QStringLiteral("freelist-vol"));
    itemInt->setMinValue(1);
    itemInt->setMaxValue(200000000);

    itemInt = addCmdItemInt(QStringLiteral("freelistBigBlocks"), m_freelistBigBlocks, 1000000, QStringLiteral("freelist-big-blocks"));
    itemInt->setMinValue(0);
    itemInt->setMaxValue(10000000);

    addCmdItemBool(QStringLiteral("undefValueErrors"), m_undefValueErrors, true, QStringLiteral("undef-value-errors"));
    addCmdItemBool(QStringLiteral("showMismatchedFrees"), m_showMismatchedFrees, true, QStringLiteral("show-mismatched-frees"));
    addCmdItemBool(QStringLiteral("partialLoadsOk"), m_partialLoadsOk, true, QStringLiteral("partial-loads-ok"));
    addCmdItemBool(QStringLiteral("trackOrigins"), m_trackOrigins, false, QStringLiteral("track-origins"));
    addCmdItemBool(QStringLiteral("expensiveDefinednessChecks"), m_expensiveDefinednessChecks, false, QStringLiteral("expensive-definedness-checks"));
}

}
