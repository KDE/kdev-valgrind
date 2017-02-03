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

#include "globalsettings.h"

namespace valgrind
{

MassifSettings::MassifSettings(const KConfigGroup& config)
    : m_config(config)
{
}

QString MassifSettings::extraParameters() const
{
    return m_config.readEntry(QStringLiteral("Massif Extra Parameters"), QString{});
}

void MassifSettings::setExtraParameters(const QString& parameters)
{
    m_config.writeEntry(QStringLiteral("Massif Extra Parameters"), parameters);
}

int MassifSettings::snapshotTreeDepth() const
{
    return m_config.readEntry(QStringLiteral("Massif Snapshot Tree Depth"), 30);
}

void MassifSettings::setSnapshotTreeDepth(int depth)
{
    m_config.writeEntry(QStringLiteral("Massif Snapshot Tree Depth"), depth);
}

int MassifSettings::threshold() const
{
    return m_config.readEntry(QStringLiteral("Massif Threshold"), 1);
}

void MassifSettings::setThreshold(int threshold)
{
    m_config.writeEntry(QStringLiteral("Massif Threshold"), threshold);
}

int MassifSettings::peakInaccuracy() const
{
    return m_config.readEntry(QStringLiteral("Massif Peak Inaccuracy"), 1);
}

void MassifSettings::setPeakInaccuracy(int inaccuracy)
{
    m_config.writeEntry(QStringLiteral("Massif Peak Inaccuracy"), inaccuracy);
}

int MassifSettings::maximumSnapshots() const
{
    return m_config.readEntry(QStringLiteral("Massif Maximum Snapshots"), 100);
}

void MassifSettings::setMaximumSnapshots(int maximum)
{
    m_config.writeEntry(QStringLiteral("Massif Maximum Snapshots"), maximum);
}

int MassifSettings::detailedSnapshotsFrequency() const
{
    return m_config.readEntry(QStringLiteral("Massif Detailed Snapshots Frequency"), 10);
}

void MassifSettings::setDetailedSnapshotsFrequency(int frequency)
{
    m_config.writeEntry(QStringLiteral("Massif Detailed Snapshots Frequency"), frequency);
}

int MassifSettings::timeUnit() const
{
    return m_config.readEntry(QStringLiteral("Massif Time Unit"), 0);
}

void MassifSettings::setTimeUnit(int unit)
{
    m_config.writeEntry(QStringLiteral("Massif Time Unit"), unit);
}

bool MassifSettings::profileHeap() const
{
    return m_config.readEntry(QStringLiteral("Massif Profile Heap"), true);
}

void MassifSettings::setProfileHeap(bool value)
{
    m_config.writeEntry(QStringLiteral("Massif Profile Heap"), value);
}

bool MassifSettings::profileStack() const
{
    return m_config.readEntry(QStringLiteral("Massif Profile Stack"), false);
}

void MassifSettings::setProfileStack(bool value)
{
    m_config.writeEntry(QStringLiteral("Massif Profile Stack"), value);
}

bool MassifSettings::launchVisualizer() const
{
    return m_config.readEntry(QStringLiteral("Massif Launch Visualizer"), false);
}

void MassifSettings::setLaunchVisualizer(bool value)
{
    m_config.writeEntry(QStringLiteral("Massif Launch Visualizer"), value);
}

QString MassifSettings::visualizerExecutablePath()
{
    return KDevelop::Path(GlobalSettings::massifVisualizerExecutablePath()).toLocalFile();
}

}
