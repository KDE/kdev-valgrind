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


#include "cachegrind_job.h"

#include "cachegrind_config.h"
#include "cachegrind_model.h"
#include "cachegrind_parser.h"
#include "cachegrind_tool.h"
#include "cachegrind_view.h"
#include "plugin.h"

#include <interfaces/ilaunchconfiguration.h>

#include <KConfigGroup>
#include <KShell>

#include <QBuffer>
#include <QFile>
#include <QTemporaryFile>

namespace Valgrind
{

CachegrindJob::CachegrindJob(KDevelop::ILaunchConfiguration* launchConfig)
    : Job(CachegrindTool::self(), launchConfig)
    , m_model(new CachegrindFunctionsModel)
    , m_outputFile(new QTemporaryFile(this))
{
    m_outputFile->open();
}

bool CachegrindJob::processEnded()
{
    CachegrindConfig config;
    config.setConfigGroup(m_configGroup);
    config.load();

    QStringList cgArgs;
    cgArgs += KShell::splitArgs(config.cgAnnotateParameters());
    cgArgs += m_outputFile->fileName();

    QByteArray cgOutput;
    if (executeProcess(config.cgAnnotateExecutablePath(), cgArgs, cgOutput) != 0) {
        return false;
    }

    cachegrindParse(cgOutput, m_model);

    return true;
}

void CachegrindJob::addToolArgs(QStringList& args) const
{
    CachegrindConfig config;
    config.setConfigGroup(m_configGroup);
    config.load();

    args += config.cmdArgs();
    args += QStringLiteral("--cachegrind-out-file=%1").arg(m_outputFile->fileName());
}

QWidget* CachegrindJob::createView()
{
    return new CachegrindView(m_model);
}

}
