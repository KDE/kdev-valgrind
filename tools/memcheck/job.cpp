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

#include "debug.h"
#include "parser.h"
#include "settings.h"

#include <interfaces/icore.h>
#include <interfaces/ilanguagecontroller.h>
#include <interfaces/ilaunchconfiguration.h>
#include <kconfiggroup.h>
#include <kshell.h>
#include <shell/problemmodel.h>
#include <shell/problemmodelset.h>
#include <KLocalizedString>

#include <QRegularExpression>
#include <unistd.h>

namespace valgrind
{

MemcheckJob::MemcheckJob(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent)
    : GenericJob(cfg,
                 QStringLiteral("memcheck"),
                 plugin,
                 parent)
{
    m_showInstructionPointer = MemcheckSettings::showInstructionPointer(cfg->config());
}

MemcheckJob::~MemcheckJob()
{
}

void MemcheckJob::postProcessStderr(const QStringList& lines)
{
    static const auto xmlStartRegex = QRegularExpression("\\s*<");

    for (const QString & line : lines) {
        if (line.isEmpty())
            continue;

        if (line.indexOf(xmlStartRegex) >= 0) // the line contains XML
            m_xmlOutput << line;
        else
            m_errorOutput << line;
    }

    KDevelop::OutputExecuteJob::postProcessStderr(lines);
}

void MemcheckJob::processEnded()
{
    auto problemModelSet = KDevelop::ICore::self()->languageController()->problemModelSet();
    auto problemModel = problemModelSet->findModel(QStringLiteral("Valgrind"));

    MemcheckParser parser;
    parser.addData(m_xmlOutput.join(" "));

    problemModel->clearProblems();
    problemModel->setProblems(parser.parse(m_showInstructionPointer));
}

void MemcheckJob::addToolArgs(QStringList& args, KConfigGroup& cfg) const
{
    args += QStringLiteral("--xml=yes");
    args += QStringLiteral("--xml-fd=%1").arg(STDERR_FILENO);

    args += KShell::splitArgs(MemcheckSettings::extraParameters(cfg));
    args += QStringLiteral("--freelist-vol=") + argValue(MemcheckSettings::freeListSize(cfg));
    args += QStringLiteral("--undef-value-errors=") + argValue(MemcheckSettings::undefValueErrors(cfg));
    args += QStringLiteral("--show-reachable=") + argValue(MemcheckSettings::showReachable(cfg));
}

QWidget* MemcheckJob::createView()
{
    return nullptr;
}

}
