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

namespace MassifSettings
{

QString extraParameters(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Massif Extra Parameters"), QString{});
}

void setExtraParameters(KConfigGroup& cfg, const QString& parameters)
{
    cfg.writeEntry(QStringLiteral("Massif Extra Parameters"), parameters);
}

int snapshotTreeDepth(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Massif Snapshot Tree Depth"), 30);
}

void setSnapshotTreeDepth(KConfigGroup& cfg, int depth)
{
    cfg.writeEntry(QStringLiteral("Massif Snapshot Tree Depth"), depth);
}

int threshold(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Massif Threshold"), 1);
}

void setThreshold(KConfigGroup& cfg, int threshold)
{
    cfg.writeEntry(QStringLiteral("Massif Threshold"), threshold);
}

int peakInaccuracy(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Massif Peak Inaccuracy"), 1);
}

void setPeakInaccuracy(KConfigGroup& cfg, int inaccuracy)
{
    cfg.writeEntry(QStringLiteral("Massif Peak Inaccuracy"), inaccuracy);
}

int maximumSnapshots(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Massif Maximum Snapshots"), 100);
}

void setMaximumSnapshots(KConfigGroup& cfg, int maximum)
{
    cfg.writeEntry(QStringLiteral("Massif Maximum Snapshots"), maximum);
}

int detailedSnapshotsFrequency(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Massif Detailed Snapshots Frequency"), 10);
}

void setDetailedSnapshotsFrequency(KConfigGroup& cfg, int frequency)
{
    cfg.writeEntry(QStringLiteral("Massif Detailed Snapshots Frequency"), frequency);
}

int timeUnit(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Massif Time Unit"), 0);
}

void setTimeUnit(KConfigGroup& cfg, int unit)
{
    cfg.writeEntry(QStringLiteral("Massif Time Unit"), unit);
}

bool profileHeap(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Massif Profile Heap"), true);
}

void setProfileHeap(KConfigGroup& cfg, bool value)
{
    cfg.writeEntry(QStringLiteral("Massif Profile Heap"), value);
}

bool profileStack(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Massif Profile Stack"), false);
}

void setProfileStack(KConfigGroup& cfg, bool value)
{
    cfg.writeEntry(QStringLiteral("Massif Profile Stack"), value);
}

bool launchVisualizer(const KConfigGroup& cfg)
{
    return cfg.readEntry(QStringLiteral("Massif Launch Visualizer"), false);
}

void setLaunchVisualizer(KConfigGroup& cfg, bool value)
{
    cfg.writeEntry(QStringLiteral("Massif Launch Visualizer"), value);
}

}

}
