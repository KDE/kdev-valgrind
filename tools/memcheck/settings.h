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

namespace valgrind
{

namespace MemcheckSettings
{

QString extraParameters(const KConfigGroup& cfg);
void setExtraParameters(KConfigGroup& cfg, const QString& parameters);

int freeListSize(const KConfigGroup& cfg);
void setFreeListSize(KConfigGroup& cfg, int size);

bool showReachable(const KConfigGroup& cfg);
void setShowReachable(KConfigGroup& cfg, bool value);

bool undefValueErrors(const KConfigGroup& cfg);
void setUndefValueErrors(KConfigGroup& cfg, bool value);

bool showInstructionPointer(const KConfigGroup& cfg);
void setShowInstructionPointer(KConfigGroup& cfg, bool value);

}

}
