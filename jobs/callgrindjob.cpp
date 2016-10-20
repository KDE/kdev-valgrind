/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>

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

#include "callgrindjob.h"

#include "job.h"
#include "debug.h"

#include <KProcess>
#include <QFile>

#include <kconfiggroup.h>
#include "debug.h"
#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <execute/iexecuteplugin.h>
#include <interfaces/icore.h>
#include <interfaces/ilaunchconfiguration.h>
#include <interfaces/iplugincontroller.h>

namespace valgrind
{
CallgrindJob::CallgrindJob(KDevelop::ILaunchConfiguration* cfg,
                           Plugin *inst,
                           QObject* parent)
    : Job(cfg, inst, parent)
    , m_file(0)
{
    m_postTreatment = 0;
}

CallgrindJob::~CallgrindJob()
{
    if (m_postTreatment != 0)
        delete m_postTreatment;
}

void CallgrindJob::processStarted()
{
    QString filename = QString("%1/callgrind.out.%2").arg(m_workingDir.toLocalFile()).arg(m_process->pid());

    m_file = new QFile(filename);
}

void CallgrindJob::processEnded()
{
    if (m_file)
    {
        KConfigGroup grp = m_launchcfg->config();

        m_postTreatment = new KProcessOutputToParser(m_parser);
        QStringList args;
        args << m_file->fileName() << "--tree=both";
        QString caPath = grp.readEntry("CallgrindAnnotateExecutable", "/usr/bin/callgrind_annotate");
        m_postTreatment->execute(caPath, args);

        if (grp.readEntry("Launch KCachegrind", false))
        {
            QStringList args;
            args << m_file->fileName();
            QString kcg = grp.readEntry("KCachegrindExecutable", "/usr/bin/kcachegrind");
            //Proxy used to remove file at the end of KCachegrind
            new QFileProxyRemove(kcg, args, m_file, (QObject *)m_plugin);
        }
        else
        {
            m_file->remove();
            delete m_file;
        }
    }
}

void CallgrindJob::addToolArgs(QStringList &args, KConfigGroup &cfg) const
{
    static const t_valgrind_cfg_argarray cg_args =
    {
        {"Callgrind Arguments", "", "str"},
        {"Callgrind Cache simulation", "--cache-sim=", "bool"},
        {"Callgrind Branch simulation", "--branch-sim=", "bool"}
    };
    static const int count = sizeof(cg_args) / sizeof(*cg_args);

    processModeArgs(args, cg_args, count, cfg);
}
}
