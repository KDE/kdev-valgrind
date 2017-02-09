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

namespace Massif
{

Settings::Settings(const KConfigGroup& config)
    : m_config(config)
{
}

QString Settings::extraParameters() const
{
    return m_config.readEntry(QStringLiteral("Massif Extra Parameters"), QString{});
}

void Settings::setExtraParameters(const QString& parameters)
{
    m_config.writeEntry(QStringLiteral("Massif Extra Parameters"), parameters);
}

int Settings::snapshotTreeDepth() const
{
    return m_config.readEntry(QStringLiteral("Massif Snapshot Tree Depth"), 30);
}

void Settings::setSnapshotTreeDepth(int depth)
{
    m_config.writeEntry(QStringLiteral("Massif Snapshot Tree Depth"), depth);
}

int Settings::threshold() const
{
    return m_config.readEntry(QStringLiteral("Massif Threshold"), 1);
}

void Settings::setThreshold(int threshold)
{
    m_config.writeEntry(QStringLiteral("Massif Threshold"), threshold);
}

int Settings::peakInaccuracy() const
{
    return m_config.readEntry(QStringLiteral("Massif Peak Inaccuracy"), 1);
}

void Settings::setPeakInaccuracy(int inaccuracy)
{
    m_config.writeEntry(QStringLiteral("Massif Peak Inaccuracy"), inaccuracy);
}

int Settings::maximumSnapshots() const
{
    return m_config.readEntry(QStringLiteral("Massif Maximum Snapshots"), 100);
}

void Settings::setMaximumSnapshots(int maximum)
{
    m_config.writeEntry(QStringLiteral("Massif Maximum Snapshots"), maximum);
}

int Settings::detailedSnapshotsFrequency() const
{
    return m_config.readEntry(QStringLiteral("Massif Detailed Snapshots Frequency"), 10);
}

void Settings::setDetailedSnapshotsFrequency(int frequency)
{
    m_config.writeEntry(QStringLiteral("Massif Detailed Snapshots Frequency"), frequency);
}

int Settings::timeUnit() const
{
    return m_config.readEntry(QStringLiteral("Massif Time Unit"), 0);
}

void Settings::setTimeUnit(int unit)
{
    m_config.writeEntry(QStringLiteral("Massif Time Unit"), unit);
}

bool Settings::profileHeap() const
{
    return m_config.readEntry(QStringLiteral("Massif Profile Heap"), true);
}

void Settings::setProfileHeap(bool value)
{
    m_config.writeEntry(QStringLiteral("Massif Profile Heap"), value);
}

bool Settings::profileStack() const
{
    return m_config.readEntry(QStringLiteral("Massif Profile Stack"), false);
}

void Settings::setProfileStack(bool value)
{
    m_config.writeEntry(QStringLiteral("Massif Profile Stack"), value);
}

bool Settings::launchVisualizer() const
{
    return m_config.readEntry(QStringLiteral("Massif Launch Visualizer"), false);
}

void Settings::setLaunchVisualizer(bool value)
{
    m_config.writeEntry(QStringLiteral("Massif Launch Visualizer"), value);
}

QString Settings::visualizerExecutablePath()
{
    return KDevelop::Path(GlobalSettings::massifVisualizerExecutablePath()).toLocalFile();
}

}

}
