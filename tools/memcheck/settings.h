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

#pragma once

#include <kconfiggroup.h>

namespace Valgrind
{

namespace Memcheck
{

class Settings
{
public:
    explicit Settings(const KConfigGroup& config);

    QString leakResolution() const;
    void setLeakResolution(const QString& resolution);

    QString showLeakKinds() const;
    void setShowLeakKinds(const QString& kinds);

    QString leakCheckHeuristics() const;
    void setLeakCheckHeuristics(const QString& heuristics);

    QString keepStacktraces() const;
    void setKeepStacktraces(const QString& keep);

    int freelistVol() const;
    void setFreelistVol(int volume);

    int freelistBigBlocks() const;
    void setFreelistBigBlocks(int size);

    QString extraParameters() const;
    void setExtraParameters(const QString& parameters);

    bool undefValueErrors() const;
    void setUndefValueErrors(bool value);

    bool showMismatchedFrees() const;
    void setShowMismatchedFrees(bool value);

    bool partialLoadsOk() const;
    void setPartialLoadsOk(bool value);

    bool trackOrigins() const;
    void setTrackOrigins(bool value);

    bool expensiveDefinednessChecks() const;
    void setExpensiveDefinednessChecks(bool value);

    bool showInstructionPointer() const;
    void setShowInstructionPointer(bool value);

private:
    KConfigGroup m_config;
};

}

}
