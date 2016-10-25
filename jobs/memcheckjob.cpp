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

#include "memcheckjob.h"

#include "debug.h"
#include "iparser.h"

#include <KLocalizedString>

#include <QRegularExpression>
#include <unistd.h>

namespace valgrind
{

MemcheckJob::MemcheckJob(KDevelop::ILaunchConfiguration* cfg, Plugin* inst, QObject* parent)
    : Job(cfg, inst, parent)
{
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

        if (line.indexOf(xmlStartRegex) >= 0) { // the line contains XML
            m_xmlOutput << line;
            m_parser->addData(line);
            m_parser->parse();
        }
        else
            m_errorOutput << line;
    }

    KDevelop::OutputExecuteJob::postProcessStderr(lines);
}

void MemcheckJob::addToolArgs(QStringList& args, KConfigGroup& cfg) const
{
    static const t_valgrind_cfg_argarray memcheck_args = {
        {QStringLiteral("Memcheck Arguments"), QStringLiteral(""), QStringLiteral("str")},
        {QStringLiteral("Freelist Size"), QStringLiteral("--freelist-vol="), QStringLiteral("int")},
        {QStringLiteral("Show Reachable"), QStringLiteral("--show-reachable="), QStringLiteral("bool")},
        {QStringLiteral("Undef Value Errors"), QStringLiteral("--undef-value-errors="), QStringLiteral("bool")}
    };

    static const int count = sizeof(memcheck_args) / sizeof(*memcheck_args);

    args << QStringLiteral("--xml=yes");
    args << QStringLiteral("--xml-fd=%1").arg(STDERR_FILENO);

    processModeArgs(args, memcheck_args, count, cfg);
}

}
