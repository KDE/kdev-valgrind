/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2016-2017 Anton Anikin <anton.anikin@htower.ru>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "job.h"

#include "debug.h"
#include "model.h"
#include "parser.h"
#include "plugin.h"
#include "settings.h"
#include "view.h"

#include <interfaces/ilaunchconfiguration.h>
#include <kshell.h>

#include <QFile>
#include <QTemporaryFile>

namespace Valgrind
{

namespace Massif
{

Job::Job(KDevelop::ILaunchConfiguration* cfg)
    : IJob(cfg, QStringLiteral("massif"), true)
    , m_model(new SnapshotsModel)
    , m_outputFile(new QTemporaryFile(this))
{
    m_outputFile->open();
}

Job::~Job()
{
}

bool Job::processEnded()
{
    Settings settings;
    settings.load(m_config);

    parse(m_outputFile->fileName(), m_model);

    return true;
}

void Job::addToolArgs(QStringList& args) const
{
    Settings settings;
    settings.load(m_config);

    int tu = settings.timeUnit;
    if (tu == 0) {
        args += QStringLiteral("--time-unit=i");
    }

    else if (tu == 1) {
        args += QStringLiteral("--time-unit=ms");
    }

    else if (tu == 2) {
        args += QStringLiteral("--time-unit=B");
    }

    args += settings.cmdArgs();
    args += QStringLiteral("--massif-out-file=%1").arg(m_outputFile->fileName());
}

QWidget* Job::createView()
{
    return new View(m_config, m_outputFile, m_model);
}

}

}
