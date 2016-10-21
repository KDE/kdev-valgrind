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

#include <QTcpServer>

namespace valgrind
{

MemcheckJob::MemcheckJob(KDevelop::ILaunchConfiguration* cfg, Plugin* inst, QObject* parent)
    : Job(cfg, inst, parent)
    , m_server(nullptr)
    , m_connection(nullptr)
{
}

MemcheckJob::~MemcheckJob()
{
    if (m_server) {
        m_server->close();
        delete m_server;
    }
}

void MemcheckJob::beforeStart()
{
    if (!m_server) {
        m_server = new QTcpServer(this);
        if (!m_server->listen()) {
            qCWarning(KDEV_VALGRIND) << "Could not open TCP socket for communication with Valgrind: "
                                     << m_server->errorString();
            delete m_server;
            m_server = nullptr;
        }

        if (m_server)
            // FIXME replace with new style
            connect(m_server, SIGNAL(newConnection()), SLOT(newValgrindConnection()));
    }
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

    args << "--xml=yes";
    if (m_server)
        args << QString("--xml-socket=127.0.0.1:%1").arg(m_server->serverPort());

    processModeArgs(args, memcheck_args, count, cfg);
}

void MemcheckJob::newValgrindConnection()
{
    QTcpSocket* sock = m_server->nextPendingConnection();
    if (!sock)
        return;

    if (m_connection) {
        qCWarning(KDEV_VALGRIND) << "Got a new valgrind connection while old one was still alive!";
        delete sock; // discard new connection
    }

    else {
        m_connection = sock;
        m_parser->setDevice(m_connection);

        // Connects the parser to the socket
        // FIXME replace with new style
        connect(m_connection, SIGNAL(readyRead()), m_parser, SLOT(parse()));
        connect(m_connection, SIGNAL(error(QAbstractSocket::SocketError)),
                SLOT(socketError(QAbstractSocket::SocketError)));
    }
}

void MemcheckJob::socketError(QAbstractSocket::SocketError)
{
    Q_ASSERT(m_connection);

    qCWarning(KDEV_VALGRIND) << i18n("Socket error while communicating with valgrind: \"%1\"", m_connection->errorString())
                             << i18n("Valgrind communication error");
}

}
