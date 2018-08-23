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

#include "settings.h"

#include "globalsettings.h"
#include "tool.h"

namespace Valgrind
{

namespace Massif
{

Settings::Settings()
    : ISettings(Tool::self()->id())

    , snapshotTreeDepth(
        this,
        QStringLiteral("Snapshot Tree Depth"),
        QStringLiteral("depth"),
        30)

    , threshold(
        this,
        QStringLiteral("Threshold"),
        QStringLiteral("threshold"),
        1)

    , peakInaccuracy(
        this,
        QStringLiteral("Peak Inaccuracy"),
        QStringLiteral("peak-inaccuracy"),
        1)

    , maximumSnapshots(
        this,
        QStringLiteral("Maximum Snapshots"),
        QStringLiteral("max-snapshots"),
        100)

    , detailedSnapshotsFrequency(
        this,
        QStringLiteral("Detailed Snapshots Frequency"),
        QStringLiteral("detailed-freq"),
        10)

    , timeUnit(
        this,
        QStringLiteral("Time Unit"),
        QStringLiteral(""),
        0)

    , profileHeap(
        this,
        QStringLiteral("Profile Heap"),
        QStringLiteral("heap"),
        true)

    , profileStack(
        this,
        QStringLiteral("Profile Stack"),
        QStringLiteral("stacks"),
        false)

    , pagesAsHeap(
        this,
        QStringLiteral("Pages As Heap"),
        QStringLiteral("pages-as-heap"),
        false)

    , launchVisualizer(
        this,
        QStringLiteral("Launch Visualizer"),
        QStringLiteral(""),
        false)
{
}

Settings::~Settings()
{
}

QString Settings::visualizerExecutablePath()
{
    return KDevelop::Path(GlobalSettings::massifVisualizerExecutablePath()).toLocalFile();
}

}

}
