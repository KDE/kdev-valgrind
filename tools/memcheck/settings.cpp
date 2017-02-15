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

QString Settings::leakResolution() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Leak Resolution"), QStringLiteral("high"));
}

void Settings::setLeakResolution(const QString& resolution)
{
    m_config.writeEntry(QStringLiteral("Memcheck Leak Resolution"), resolution);
}

QString Settings::showLeakKinds() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Show Leak Kinds"), QStringLiteral("definite,possible"));
}

void Settings::setShowLeakKinds(const QString& kinds)
{
    m_config.writeEntry(QStringLiteral("Memcheck Show Leak Kinds"), QString(kinds).remove(QChar(' ')));
}

QString Settings::leakCheckHeuristics() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Leak Check Heuristics"), QStringLiteral("all"));
}

void Settings::setLeakCheckHeuristics(const QString& heuristics)
{
    m_config.writeEntry(QStringLiteral("Memcheck Leak Check Heuristics"), QString(heuristics).remove(QChar(' ')));
}

QString Settings::keepStacktraces() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Keep Stacktraces"), QStringLiteral("alloc-and-free"));
}

void Settings::setKeepStacktraces(const QString& keep)
{
    m_config.writeEntry(QStringLiteral("Memcheck Keep Stacktraces"), keep);
}

int Settings::freelistVol() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Freelist Volume"), 20000000);
}

void Settings::setFreelistVol(int volume)
{
    m_config.writeEntry(QStringLiteral("Memcheck Freelist Volume"), volume);
}

int Settings::freelistBigBlocks() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Freelist Big Blocks"), 1000000);
}

void Settings::setFreelistBigBlocks(int size)
{
    m_config.writeEntry(QStringLiteral("Memcheck Freelist Big Blocks"), size);
}

QString Settings::extraParameters() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Extra Parameters"), QString{});
}

void Settings::setExtraParameters(const QString& parameters)
{
    m_config.writeEntry(QStringLiteral("Memcheck Extra Parameters"), parameters);
}

bool Settings::undefValueErrors() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Undef Value Errors"), true);
}

void Settings::setUndefValueErrors(bool value)
{
    m_config.writeEntry(QStringLiteral("Memcheck Undef Value Errors"), value);
}

bool Settings::showMismatchedFrees() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Show Mismatched Frees"), true);
}

void Settings::setShowMismatchedFrees(bool value)
{
    m_config.writeEntry(QStringLiteral("Memcheck Show Mismatched Frees"), value);
}

bool Settings::partialLoadsOk() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Partial Loads Ok"), true);
}

void Settings::setPartialLoadsOk(bool value)
{
    m_config.writeEntry(QStringLiteral("Memcheck Partial Loads Ok"), value);
}

bool Settings::trackOrigins() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Track Origins"), false);
}

void Settings::setTrackOrigins(bool value)
{
    m_config.writeEntry(QStringLiteral("Memcheck Track Origins"), value);
}

bool Settings::expensiveDefinednessChecks() const
{
    return m_config.readEntry(QStringLiteral("Memcheck Expensive Definedness Checks"), false);
}

void Settings::setExpensiveDefinednessChecks(bool value)
{
    m_config.writeEntry(QStringLiteral("Memcheck Expensive Definedness Checks"), value);
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
