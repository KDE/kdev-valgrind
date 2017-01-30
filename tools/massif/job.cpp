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
#include "view.h"

#include "debug.h"
#include "interfaces/iparser.h"
#include "plugin.h"
#include "globalsettings.h"

#include <QFile>
#include <QTcpServer>
#include <QTcpSocket>
#include <KProcess>
#include <kconfiggroup.h>
#include "debug.h"
#include <klocalizedstring.h>

#include <execute/iexecuteplugin.h>
#include <interfaces/icore.h>
#include <interfaces/ilaunchconfiguration.h>
#include <interfaces/iplugincontroller.h>

namespace valgrind
{
MassifJob::MassifJob(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent)
    : IJob(cfg,
           QStringLiteral("massif"),
           new MassifModel(),
           new MassifParser(),
           plugin,
           parent)
    , m_outputFile(QStringLiteral("%1/kdevvalgrind_massif.out").arg(m_workingDir.toLocalFile()))
{
}

MassifJob::~MassifJob()
{
}

void MassifJob::processEnded()
{
    KConfigGroup config = m_launchcfg->config();

    QFile massifResults(m_outputFile);
    massifResults.open(QIODevice::ReadOnly);

    m_parser->setDevice(&massifResults);
    m_parser->parse();
    massifResults.close();

    if (config.readEntry("launchVisualizer", false)) {
        QStringList args;
        args += massifResults.fileName();
        QString mv = KDevelop::Path(GlobalSettings::massifVisualizerExecutablePath()).toLocalFile();
        new QFileProxyRemove(mv, args, massifResults.fileName(), dynamic_cast<QObject*>(m_plugin));
    } else
        massifResults.remove();
}

void MassifJob::addToolArgs(QStringList& args, KConfigGroup& cfg) const
{
    static const t_valgrind_cfg_argarray massifArgs = {
        {QStringLiteral("Massif Arguments"), QStringLiteral(""), QStringLiteral("str")},
        {QStringLiteral("depth"), QStringLiteral("--depth="), QStringLiteral("int")},
        {QStringLiteral("threshold"), QStringLiteral("--threshold="), QStringLiteral("float")},
        {QStringLiteral("peakInaccuracy"), QStringLiteral("--peak-inaccuracy="), QStringLiteral("float")},
        {QStringLiteral("maxSnapshots"), QStringLiteral("--max-snapshots="), QStringLiteral("int")},
        {QStringLiteral("snapshotFreq"), QStringLiteral("--detailed-freq="), QStringLiteral("int")},
        {QStringLiteral("profileHeap"), QStringLiteral("--heap="), QStringLiteral("bool")},
        {QStringLiteral("profileStack"), QStringLiteral("--stacks="), QStringLiteral("bool")}
    };
    static const int count = sizeof(massifArgs) / sizeof(*massifArgs);


    int tu = cfg.readEntry(QStringLiteral("timeUnit"), 0);

    if (tu == 0)
        args += QStringLiteral("--time-unit=i");

    else if (tu == 1)
        args += QStringLiteral("--time-unit=ms");

    else if (tu == 2)
        args += QStringLiteral("--time-unit=B");

    args += QStringLiteral("--massif-out-file=%1").arg(m_outputFile);

    processModeArgs(args, massifArgs, count, cfg);
}

IView* MassifJob::createView()
{
    return new MassifView;
}

}
