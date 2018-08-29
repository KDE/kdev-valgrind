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

#pragma once

#include "config.h"

namespace Valgrind
{

class MassifConfig : public Config
{
public:
    MassifConfig();
    ~MassifConfig() override = default;

    static QString visualizerExecutablePath();

    bool launchVisualizer() const
    {
        return m_launchVisualizer;
    }

private:
    int m_snapshotTreeDepth;
    int m_maximumSnapshots;
    int m_detailedSnapshotsFrequency;

    double m_threshold;
    double m_peakInaccuracy;

    QString m_timeUnit;

    bool m_profileHeap;
    bool m_profileStack;
    bool m_pagesAsHeap;

    bool m_launchVisualizer;
};

}
