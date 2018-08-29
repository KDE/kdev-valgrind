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

#include "massif_config.h"

#include "globalsettings.h"
#include "massif_tool.h"

namespace Valgrind
{

MassifConfig::MassifConfig()
    : Config(MassifTool::self()->id())
{
    auto itemInt = addCmdItemInt(QStringLiteral("snapshotTreeDepth"), m_snapshotTreeDepth, 30, QStringLiteral("depth"));
    itemInt->setMinValue(1);
    itemInt->setMaxValue(3000);

    auto itemDouble = addCmdItemDouble(QStringLiteral("threshold"), m_threshold, 1.0, QStringLiteral("threshold"));
    itemDouble->setMinValue(1.0);
    itemDouble->setMaxValue(100.0);

    itemDouble = addCmdItemDouble(QStringLiteral("peakInaccuracy"), m_peakInaccuracy, 1.0, QStringLiteral("peak-inaccuracy"));
    itemDouble->setMinValue(0.0);
    itemDouble->setMaxValue(100.0);

    itemInt = addCmdItemInt(QStringLiteral("maximumSnapshots"), m_maximumSnapshots, 100, QStringLiteral("max-snapshots"));
    itemInt->setMinValue(1);
    itemInt->setMaxValue(10000);

    itemInt = addCmdItemInt(QStringLiteral("detailedSnapshotsFrequency"), m_detailedSnapshotsFrequency, 10, QStringLiteral("detailed-freq"));
    itemInt->setMinValue(1);
    itemInt->setMaxValue(10000);

    addCmdItemString(QStringLiteral("timeUnit"), m_timeUnit, QStringLiteral("i"), QStringLiteral("time-unit"));

    addCmdItemBool(QStringLiteral("profileHeap"), m_profileHeap, true, QStringLiteral("heap"));
    addCmdItemBool(QStringLiteral("profileStack"), m_profileStack, false, QStringLiteral("stacks"));
    addCmdItemBool(QStringLiteral("pagesAsHeap"), m_pagesAsHeap, false, QStringLiteral("pages-as-heap"));

    addItemBool(QStringLiteral("launchVisualizer"), m_launchVisualizer, false);
}

QString MassifConfig::visualizerExecutablePath()
{
    return KDevelop::Path(GlobalSettings::massifVisualizerExecutablePath()).toLocalFile();
}

}
