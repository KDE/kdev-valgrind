/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
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

#include "callgrind_job.h"

#include "callgrind_config.h"
#include "callgrind_model.h"
#include "callgrind_parser.h"
#include "callgrind_tool.h"
#include "callgrind_view.h"
#include "plugin.h"

#include <interfaces/ilaunchconfiguration.h>

#include <KConfigGroup>
#include <KShell>

#include <QBuffer>
#include <QFile>
#include <QTemporaryFile>

namespace Valgrind
{

CallgrindJob::CallgrindJob(const LaunchInfo& launchInfo)
    : Job(CallgrindTool::self(), launchInfo)
    , m_model(new CallgrindFunctionsModel(this))
    , m_outputFile(new QTemporaryFile(this))
{
    m_outputFile->open();
}

bool CallgrindJob::processEnded()
{
    CallgrindConfig config;
    config.setConfigGroup(m_configGroup);
    config.load();

    QStringList caArgs;
    caArgs += KShell::splitArgs(config.callgrindAnnotateArgs());
    caArgs += QStringLiteral("--tree=calling");
    caArgs += QStringLiteral("--threshold=100");
    caArgs += m_outputFile->fileName();

    QByteArray caOutput;
    if (executeProcess(config.callgrindAnnotateExecutablePath(), caArgs, caOutput)) {
        return false;
    }

    callgrindParse(caOutput, m_model);

    return true;
}

void CallgrindJob::addToolArgs(QStringList& args) const
{
    CallgrindConfig config;
    config.setConfigGroup(m_configGroup);
    config.load();

    args += config.cmdArgs();
    args += QStringLiteral("--callgrind-out-file=%1").arg(m_outputFile->fileName());
}

QWidget* CallgrindJob::createView()
{
    return new CallgrindView(m_configGroup, m_outputFile, m_model);
}

}
