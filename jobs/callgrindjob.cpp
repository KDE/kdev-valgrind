/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
   Copyright 2016 Anton Anikin <anton.anikin@htower.ru>

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

#include "callgrindjob.h"

#include "plugin.h"

#include <interfaces/ilaunchconfiguration.h>
#include <kconfiggroup.h>

#include <QFile>

namespace valgrind
{

CallgrindJob::CallgrindJob(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent)
    : Job(cfg, plugin, parent)
    , m_postTreatment(new KProcessOutputToParser(m_parser))
    , m_outputFile(new QFile)
{
    m_outputFile->setFileName(QStringLiteral("%1/kdevvalgrind_callgrind.out").arg(m_workingDir.toLocalFile()));
}

CallgrindJob::~CallgrindJob()
{
    delete m_postTreatment;
    delete m_outputFile;
}

void CallgrindJob::processEnded()
{
    KConfigGroup config = m_launchcfg->config();

    QStringList args;
    args += m_outputFile->fileName();
    args += QStringLiteral("--tree=both");

    QString caPath = config.readEntry(QStringLiteral("CallgrindAnnotateExecutable"),
                                      QStringLiteral("/usr/bin/callgrind_annotate"));

    m_postTreatment->execute(caPath, args);

    if (config.readEntry(QStringLiteral("Launch KCachegrind"), false)) {
        args.clear();
        args += m_outputFile->fileName();

        QString kcg = config.readEntry(QStringLiteral("KCachegrindExecutable"),
                                       QStringLiteral("/usr/bin/kcachegrind"));

        // Proxy used to remove file at the end of KCachegrind
        new QFileProxyRemove(kcg, args, m_outputFile->fileName(), dynamic_cast<QObject*>(m_plugin));
    }
    else
        m_outputFile->remove();
}

void CallgrindJob::addToolArgs(QStringList& args, KConfigGroup& cfg) const
{
    static const t_valgrind_cfg_argarray cgArgs = {
        {QStringLiteral("Callgrind Arguments"), QStringLiteral(""), QStringLiteral("str")},
        {QStringLiteral("Callgrind Cache simulation"), QStringLiteral("--cache-sim="), QStringLiteral("bool")},
        {QStringLiteral("Callgrind Branch simulation"), QStringLiteral("--branch-sim="), QStringLiteral("bool")}
    };
    static const int count = sizeof(cgArgs) / sizeof(*cgArgs);

    args << QStringLiteral("--callgrind-out-file=%1").arg(m_outputFile->fileName());

    processModeArgs(args, cgArgs, count, cfg);
}

}
