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

class MassifSettings
{
public:
    explicit MassifSettings(const KConfigGroup& config);

    QString extraParameters() const;
    void setExtraParameters(const QString& parameters);

    int snapshotTreeDepth() const;
    void setSnapshotTreeDepth(int depth);

    int threshold() const;
    void setThreshold(int threshold);

    int peakInaccuracy() const;
    void setPeakInaccuracy(int inaccuracy);

    int maximumSnapshots() const;
    void setMaximumSnapshots(int maximum);

    int detailedSnapshotsFrequency() const;
    void setDetailedSnapshotsFrequency(int frequency);

    int timeUnit() const;
    void setTimeUnit(int unit);

    bool profileHeap() const;
    void setProfileHeap(bool value);

    bool profileStack() const;
    void setProfileStack(bool value);

    bool launchVisualizer() const;
    void setLaunchVisualizer(bool value);

    static QString visualizerExecutablePath();

private:
    KConfigGroup m_config;
};

}
