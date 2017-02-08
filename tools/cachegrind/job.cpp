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

#include "model.h"
#include "parser.h"
#include "plugin.h"
#include "settings.h"
#include "view.h"

#include <interfaces/ilaunchconfiguration.h>
#include <kconfiggroup.h>
#include <kshell.h>

#include <QBuffer>
#include <QFile>

namespace valgrind
{

CachegrindJob::CachegrindJob(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent)
    : GenericJob(cfg, QStringLiteral("cachegrind"), true, plugin, parent)
    , m_model(new CachegrindModel)
    , m_outputFile(QStringLiteral("%1/kdevvalgrind_cachegrind.out").arg(m_workingDir.toLocalFile()))
{
}

CachegrindJob::~CachegrindJob()
{
}

bool CachegrindJob::processEnded()
{
    CachegrindSettings settings(config);

    QStringList cgArgs;
    cgArgs += argValue(settings.cgAnnotateParameters());
    cgArgs += m_outputFile;

    QByteArray cgOutput;
    if (executeProcess(settings.cgAnnotateExecutablePath(), cgArgs, cgOutput) != 0) {
        return false;
    }

    CachegrindParser parser;
    parser.parse(cgOutput, m_model);
    QFile::remove(m_outputFile);

    return true;
}

void CachegrindJob::addToolArgs(QStringList& args) const
{
    CachegrindSettings settings(config);

    args += QStringLiteral("--cachegrind-out-file=%1").arg(m_outputFile);

    args += argValue(settings.extraParameters());
    args += QStringLiteral("--cache-sim=") + argValue(settings.cacheSimulation());
    args += QStringLiteral("--branch-sim=") + argValue(settings.branchSimulation());
}

QWidget* CachegrindJob::createView()
{
    return new CachegrindView(m_model);
}

}
