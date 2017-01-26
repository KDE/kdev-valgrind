/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
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


#include "cachegrindjob.h"

#include "plugin.h"

#include <interfaces/ilaunchconfiguration.h>
#include <kconfiggroup.h>

#include <QFile>

namespace valgrind
{

CachegrindJob::CachegrindJob(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent)
    : IJob(cfg, plugin, parent)
    , m_postTreatment(new KProcessOutputToParser(m_parser))
    , m_outputFile(QStringLiteral("%1/kdevvalgrind_cachegrind.out").arg(m_workingDir.toLocalFile()))
{
}

CachegrindJob::~CachegrindJob()
{
    delete m_postTreatment;
}

void CachegrindJob::processEnded()
{
    KConfigGroup config = m_launchcfg->config();

    QStringList args(m_outputFile);

    QString caPath = config.readEntry(QStringLiteral("CachegrindAnnotateExecutable"),
                                      QStringLiteral("/usr/bin/cg_annotate"));

    m_postTreatment->execute(caPath, args);

    if (config.readEntry(QStringLiteral("Launch KCachegrind"), false)) {
        args.clear();
        args += m_outputFile;

        QString kcg = config.readEntry(QStringLiteral("KCachegrindExecutable"),
                                       QStringLiteral("/usr/bin/kcachegrind"));

        //Proxy used to remove file at the end of KCachegrind
        new QFileProxyRemove(kcg, args, m_outputFile, dynamic_cast<QObject*>(m_plugin));
    }
    else
        QFile::remove(m_outputFile);
}

void CachegrindJob::addToolArgs(QStringList& args, KConfigGroup& cfg) const
{
    static const t_valgrind_cfg_argarray cgArgs = {
        {QStringLiteral("Cachegrind Arguments"), QStringLiteral(""), QStringLiteral("str")},
        {QStringLiteral("Cachegrind Cache simulation"), QStringLiteral("--cache-sim="), QStringLiteral("bool")},
        {QStringLiteral("Cachegrind Branch simulation"), QStringLiteral("--branch-sim="), QStringLiteral("bool")}
    };
    static const int count = sizeof(cgArgs) / sizeof(*cgArgs);

    args << QStringLiteral("--cachegrind-out-file=%1").arg(m_outputFile);

    processModeArgs(args, cgArgs, count, cfg);
}

}
