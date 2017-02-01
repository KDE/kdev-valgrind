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


#include "job.h"

#include "model.h"
#include "parser.h"
#include "settings.h"
#include "view.h"

#include "plugin.h"
#include "globalsettings.h"

#include <interfaces/ilaunchconfiguration.h>
#include <kconfiggroup.h>

#include <QBuffer>
#include <QFile>

namespace valgrind
{

CachegrindJob::CachegrindJob(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent)
    : IJob(cfg,
           QStringLiteral("cachegrind"),
           new CachegrindModel(),
           plugin,
           parent)
    , m_outputFile(QStringLiteral("%1/kdevvalgrind_cachegrind.out").arg(m_workingDir.toLocalFile()))
{
}

CachegrindJob::~CachegrindJob()
{
}

void CachegrindJob::processEnded()
{
    KConfigGroup config = m_launchcfg->config();

    QString cgPath = KDevelop::Path(GlobalSettings::cg_annotateExecutablePath()).toLocalFile();
    QStringList args(m_outputFile);

    {
        CachegrindParser parser;
        connect(&parser, &CachegrindParser::newItem, this, [this](ModelItem* item){
            m_model->newItem(item);
        });

        QByteArray cgOutput;
        executeProcess(cgPath, args, cgOutput);
        parser.parse(cgOutput);
    }

    if (CachegrindSettings::launchKCachegrind(config)) {
        args.clear();
        args += m_outputFile;

        QString kcg = KDevelop::Path(GlobalSettings::kcachegrindExecutablePath()).toLocalFile();

        // Proxy used to remove file at the end of KCachegrind
        new QFileProxyRemove(kcg, args, m_outputFile, dynamic_cast<QObject*>(m_plugin));
    }
    else
        QFile::remove(m_outputFile);
}

void CachegrindJob::addToolArgs(QStringList& args, KConfigGroup& cfg) const
{
    args += QStringLiteral("--cachegrind-out-file=%1").arg(m_outputFile);

    args += CachegrindSettings::extraParameters(cfg);
    args += QStringLiteral("--cache-sim=") + argValue(CachegrindSettings::cacheSimulation(cfg));
    args += QStringLiteral("--branch-sim=") + argValue(CachegrindSettings::branchSimulation(cfg));
}

IView* CachegrindJob::createView()
{
    return new CachegrindView;
}

}
