/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2009 Andreas Pakulat <apaku@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "valgrindconfig.h"

#include <kicon.h>
#include <klocale.h>

#include "valgrindjob.h"

CacheGrindLaunchMode::CacheGrindLaunchMode()
{
}

KIcon CacheGrindLaunchMode::icon() const
{
    return KIcon();
}


QString CacheGrindLaunchMode::id() const
{
    return "valgrind_cachegrind";
}


QString CacheGrindLaunchMode::name() const
{
    return i18n("Cache Simulator");
}


QString CacheGrindLaunchMode::tool() const
{
    return "cachegrind";
}

CallGrindLaunchMode::CallGrindLaunchMode()
{
}


KIcon CallGrindLaunchMode::icon() const
{
    return KIcon();
}


QString CallGrindLaunchMode::id() const
{
    return "valgrind_callgrind";
}


QString CallGrindLaunchMode::name() const
{
    return i18n("Call Tracing");
}

QString CallGrindLaunchMode::tool() const
{
    return "callgrind";
}

HelGrindLaunchMode::HelGrindLaunchMode()
{
}


KIcon HelGrindLaunchMode::icon() const
{
    return KIcon();
}


QString HelGrindLaunchMode::id() const
{
    return "valgrind_helgrind";
}


QString HelGrindLaunchMode::name() const
{
    return i18n("Race Conditions");
}


QString HelGrindLaunchMode::tool() const
{
    return "helgrind";
}

MemCheckLaunchMode::MemCheckLaunchMode()
{
}


KIcon MemCheckLaunchMode::icon() const
{
    return KIcon();
}

QString MemCheckLaunchMode::id() const
{
    return "valgrind_memcheck";
}


QString MemCheckLaunchMode::name() const
{
    return i18n("Memory Check");
}


QString MemCheckLaunchMode::tool() const
{
    return "memcheck";
}
