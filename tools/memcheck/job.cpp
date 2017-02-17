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
#include "generic/xmlparser.h"
#include "plugin.h"
#include "settings.h"

#include <interfaces/icore.h>
#include <interfaces/ilanguagecontroller.h>
#include <interfaces/ilaunchconfiguration.h>
#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <kshell.h>
#include <shell/problemmodel.h>

#include <QRegularExpression>
#include <unistd.h>

namespace Valgrind
{

namespace Memcheck
{

Job::Job(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent)
    : Generic::Job(cfg, QStringLiteral("memcheck"), false, plugin, parent)
{
}

Job::~Job()
{
}

void Job::postProcessStderr(const QStringList& lines)
{
    static const auto xmlStartRegex = QRegularExpression("\\s*<");

    for (const QString & line : lines) {
        if (line.isEmpty()) {
            continue;
        }

        if (line.indexOf(xmlStartRegex) >= 0) { // the line contains XML
            m_xmlOutput << line;
        } else {
            m_errorOutput << line;
        }
    }

    KDevelop::OutputExecuteJob::postProcessStderr(lines);
}

bool Job::processEnded()
{
    Settings settings;
    settings.load(m_config);

    m_plugin->problemModel()->setProblems(XmlParser::parse(m_xmlOutput.join(" "),
                                                           settings.showInstructionPointer));

    return true;
}

void Job::addToolArgs(QStringList& args) const
{
    Settings settings;
    settings.load(m_config);

    args += QStringLiteral("--xml=yes");
    args += QStringLiteral("--xml-fd=%1").arg(STDERR_FILENO);
    args += settings.cmdArgs();
}

QWidget* Job::createView()
{
    return nullptr;
}

}

}
