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
#include "settings.h"
#include "view.h"

#include "debug.h"
#include "generic/utils.h"
#include "plugin.h"
#include "globalsettings.h"

#include <QFile>
#include <QTcpServer>
#include <QTcpSocket>
#include <KProcess>
#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <kshell.h>

#include <execute/iexecuteplugin.h>
#include <interfaces/icore.h>
#include <interfaces/ilaunchconfiguration.h>
#include <interfaces/iplugincontroller.h>

namespace valgrind
{
MassifJob::MassifJob(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent)
    : GenericJob(cfg,
                 QStringLiteral("massif"),
                 plugin,
                 parent)
    , m_model(new MassifModel)
    , m_outputFile(QStringLiteral("%1/kdevvalgrind_massif.out").arg(m_workingDir.toLocalFile()))
{
}

MassifJob::~MassifJob()
{
}

void MassifJob::processEnded()
{
    MassifParser::parse(m_outputFile, m_model);

    if (MassifSettings::launchVisualizer(m_launchcfg->config())) {
        QStringList args;
        args += m_outputFile;
        QString mv = KDevelop::Path(GlobalSettings::massifVisualizerExecutablePath()).toLocalFile();
        new QFileProxyRemove(mv, args, m_outputFile, m_plugin);
    } else
        QFile::remove(m_outputFile);
}

void MassifJob::addToolArgs(QStringList& args, KConfigGroup& cfg) const
{
    int tu = MassifSettings::timeUnit(cfg);
    if (tu == 0)
        args += QStringLiteral("--time-unit=i");

    else if (tu == 1)
        args += QStringLiteral("--time-unit=ms");

    else if (tu == 2)
        args += QStringLiteral("--time-unit=B");

    args += QStringLiteral("--massif-out-file=%1").arg(m_outputFile);

    args += KShell::splitArgs(MassifSettings::extraParameters(cfg));
    args += QStringLiteral("--depth=") + argValue(MassifSettings::detailedSnapshotsFrequency(cfg));
    args += QStringLiteral("--threshold=") + argValue(MassifSettings::threshold(cfg));
    args += QStringLiteral("--peak-inaccuracy=") + argValue(MassifSettings::peakInaccuracy(cfg));
    args += QStringLiteral("--max-snapshots=") + argValue(MassifSettings::maximumSnapshots(cfg));
    args += QStringLiteral("--detailed-freq=") + argValue(MassifSettings::detailedSnapshotsFrequency(cfg));
    args += QStringLiteral("--heap=") + argValue(MassifSettings::profileHeap(cfg));
    args += QStringLiteral("--stacks=") + argValue(MassifSettings::profileStack(cfg));
}

QWidget* MassifJob::createView()
{
    auto view = new MassifView;
    view->setModel(m_model);
    return view;
}

}
