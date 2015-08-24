/* This file is part of KDevelop
 * Copyright (C) 2015 Laszlo Kis-Adam <laszlo.kis-adam@kdemail.net>

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


#include "statjob.h"

#include <kio/statjob.h>
#include <KJobWidgets>

namespace StatJob
{

    bool jobExists(const QUrl& url, QWidget* parent)
    {
        auto job = KIO::stat(url, KIO::StatJob::SourceSide, 0 );
        KJobWidgets::setWindow(job, parent);
        return job->exec();
    }

}

