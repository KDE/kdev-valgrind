/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2016-2017 Anton Anikin <anton@anikin.xyz>

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

#include "massif_job.h"

#include "debug.h"
#include "massif_config.h"
#include "massif_model.h"
#include "massif_parser.h"
#include "massif_tool.h"
#include "massif_view.h"
#include "plugin.h"

#include <interfaces/ilaunchconfiguration.h>

#include <KShell>

#include <QFile>
#include <QTemporaryFile>

namespace Valgrind
{

MassifJob::MassifJob(const LaunchInfo& launchInfo)
    : Job(MassifTool::self(), launchInfo)
    , m_model(new MassifSnapshotsModel)
    , m_outputFile(new QTemporaryFile(this))
{
    m_outputFile->open();
}

bool MassifJob::processEnded()
{
    MassifConfig config;
    config.setConfigGroup(m_configGroup);
    config.load();

    massifParse(m_outputFile->fileName(), m_model);

    return true;
}

void MassifJob::addToolArgs(QStringList& args) const
{
    MassifConfig config;
    config.setConfigGroup(m_configGroup);
    config.load();

    args += config.cmdArgs();
    args += QStringLiteral("--massif-out-file=%1").arg(m_outputFile->fileName());
}

QWidget* MassifJob::createView()
{
    return new MassifView(m_configGroup, m_outputFile, m_model);
}

}
