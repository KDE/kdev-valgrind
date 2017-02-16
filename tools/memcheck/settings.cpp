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

Settings::Settings()
    : ISettings(QStringLiteral("Memcheck"))

    , leakResolution(
        this,
        QStringLiteral("Leak Resolution"),
        QStringLiteral("leak-resolution"),
        QStringLiteral("high"))

    , showLeakKinds(
        this,
        QStringLiteral("Show Leak Kinds"),
        QStringLiteral("show-leak-kinds"),
        QStringLiteral("definite,possible"))

    , leakCheckHeuristics(
        this,
        QStringLiteral("Leak Check Heuristics"),
        QStringLiteral("leak-check-heuristics"),
        QStringLiteral("all"))

    , keepStacktraces(
        this,
        QStringLiteral("Keep Stacktraces"),
        QStringLiteral("keep-stacktraces"),
        QStringLiteral("alloc-and-free"))

    , freelistVol(
        this,
        QStringLiteral("Freelist Volume"),
        QStringLiteral("freelist-vol"),
        20000000)

    , freelistBigBlocks(
        this,
        QStringLiteral("Freelist Big Blocks"),
        QStringLiteral("freelist-big-blocks"),
        1000000)

    , undefValueErrors(
        this,
        QStringLiteral("Undef Value Errors"),
        QStringLiteral("undef-value-errors"),
        true)

    , showMismatchedFrees(
        this,
        QStringLiteral("Show Mismatched Frees"),
        QStringLiteral("show-mismatched-frees"),
        true)

    , partialLoadsOk(
        this,
        QStringLiteral("Partial Loads Ok"),
        QStringLiteral("partial-loads-ok"),
        true)

    , trackOrigins(
        this,
        QStringLiteral("Track Origins"),
        QStringLiteral("track-origins"),
        false)

    , expensiveDefinednessChecks(
        this,
        QStringLiteral("Expensive Definedness Checks"),
        QStringLiteral("expensive-definedness-checks"),
        false)

    , showInstructionPointer(
        this,
        QStringLiteral("Show Instruction Pointer"),
        QStringLiteral(""),
        false)
{
}

Settings::~Settings()
{
}

}

}
