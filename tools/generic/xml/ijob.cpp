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

#include "ijob.h"

#include "debug.h"
#include "parser.h"
#include "plugin.h"
#include "isettings.h"

#include <shell/problemmodel.h>

#include <QRegularExpression>
#include <unistd.h>

namespace Valgrind
{

IXmlJob::IXmlJob(
    KDevelop::ILaunchConfiguration* cfg,
    const QString& toolName,
    IXmlSettings* settings,
    Plugin* plugin,
    QObject* parent)

    : Generic::Job(cfg, toolName, false, plugin, parent)
    , m_settings(settings)
{
    Q_ASSERT(m_settings);
}

IXmlJob::~IXmlJob()
{
    delete m_settings;
}

void IXmlJob::processValgrindOutput(const QStringList& lines)
{
    static const auto xmlStartRegex = QRegularExpression("\\s*<");

    for (const QString & line : lines) {
        if (line.isEmpty()) {
            continue;
        }

        if (line.indexOf(xmlStartRegex) >= 0) { // the line contains XML
            m_xmlOutput << line;
        }
    }

    Generic::Job::processValgrindOutput(lines);
}

bool IXmlJob::processEnded()
{
    m_settings->load(m_config);

    auto problems = XmlParser::parse(m_xmlOutput.join(" "), m_settings->showInstructionPointer);
    m_plugin->problemModel()->setProblems(problems);

    return true;
}

void IXmlJob::addLoggingArgs(QStringList& args) const
{
    args += QStringLiteral("--xml=yes");
    args += QStringLiteral("--xml-socket=127.0.0.1:%1").arg(m_tcpServerPort);
}

void IXmlJob::addToolArgs(QStringList& args) const
{
    m_settings->load(m_config);
    args += m_settings->cmdArgs();
}

QWidget* IXmlJob::createView()
{
    return nullptr;
}

}
