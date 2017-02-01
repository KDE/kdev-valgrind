/* This file is part of KDevelop
   Copyright 2015 Laszlo Kis-Adam <laszlo.kis-adam@kdemail.net>
   Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#include "utils.h"

#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <kio/statjob.h>
#include <KJobWidgets>

#include <QApplication>
#include <QFile>
#include <QProcess>

namespace valgrind
{

QFileProxyRemove::QFileProxyRemove(
    const QString& programPath,
    const QStringList& args,
    const QString& fileName,
    QObject* parent)

    : QObject(parent)
    , m_file(new QFile(fileName))
    , m_process(new QProcess(this))
    , m_execPath(programPath)
{
    connect(m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, [this](int, QProcess::ExitStatus) {
        deleteLater();
    });

    connect(m_process, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
            this, [this](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart)
            KMessageBox::error(qApp->activeWindow(),
                               i18n("Unable to launch the process %1 (%2)", m_execPath, error),
                               i18n("Valgrind Error"));
        deleteLater();
    });

    m_process->start(programPath, args);
}

QFileProxyRemove::~QFileProxyRemove()
{
    m_file->remove();
    delete m_file;
    delete m_process;
}

namespace StatJob
{

bool jobExists(const QUrl& url, QWidget* parent)
{
    auto job = KIO::stat(url, KIO::StatJob::SourceSide, 0);
    KJobWidgets::setWindow(job, parent);
    return job->exec();
}

}

}
