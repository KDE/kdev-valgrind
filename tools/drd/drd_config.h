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

#include "xmlconfig.h"

namespace Valgrind
{

class DrdConfig : public XmlConfig
{
    Q_OBJECT

public:
    DrdConfig();
    ~DrdConfig() override = default;

private:
    bool m_checkStackVar;
    bool m_firstRaceOnly;
    bool m_freeIsWrite;
    bool m_reportSignalUnlocked;
    bool m_segmentMerging;
    bool m_showConflSeg;
    bool m_showStackUsage;
    bool m_ignoreThreadCreation;
    bool m_traceAlloc;
    bool m_traceBarrier;
    bool m_traceCond;
    bool m_traceForkJoin;
    bool m_traceHb;
    bool m_traceMutex;
    bool m_traceRwlock;
    bool m_traceSemaphore;

    int m_joinListVol;
    int m_segmentMergingInterval;
};

}
