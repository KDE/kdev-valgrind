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

namespace MassifSettings
{

QString extraParameters(const KConfigGroup& cfg);
void setExtraParameters(KConfigGroup& cfg, const QString& parameters);

int snapshotTreeDepth(const KConfigGroup& cfg);
void setSnapshotTreeDepth(KConfigGroup& cfg, int depth);

int threshold(const KConfigGroup& cfg);
void setThreshold(KConfigGroup& cfg, int threshold);

int peakInaccuracy(const KConfigGroup& cfg);
void setPeakInaccuracy(KConfigGroup& cfg, int inaccuracy);

int maximumSnapshots(const KConfigGroup& cfg);
void setMaximumSnapshots(KConfigGroup& cfg, int maximum);

int detailedSnapshotsFrequency(const KConfigGroup& cfg);
void setDetailedSnapshotsFrequency(KConfigGroup& cfg, int frequency);

int timeUnit(const KConfigGroup& cfg);
void setTimeUnit(KConfigGroup& cfg, int unit);

bool profileHeap(const KConfigGroup& cfg);
void setProfileHeap(KConfigGroup& cfg, bool value);

bool profileStack(const KConfigGroup& cfg);
void setProfileStack(KConfigGroup& cfg, bool value);

bool launchVisualizer(const KConfigGroup& cfg);
void setLaunchVisualizer(KConfigGroup& cfg, bool value);

}

}
