/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2002 Harald Fernengel <harry@kdevelop.org>
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
#include "globalsettings.h"
#include "plugin.h"
#include "settings.h"

#include <execute/iexecuteplugin.h>
#include <interfaces/icore.h>
#include <interfaces/ilaunchconfiguration.h>
#include <interfaces/iplugincontroller.h>
#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <kshell.h>
#include <util/environmentprofilelist.h>

#include <QApplication>
#include <QBuffer>
#include <QFileInfo>

#include <QTcpServer>
#include <QTcpSocket>


namespace Valgrind
{

static const QString valgrindErrorsPrefix = QStringLiteral("valgrind: ");

IJob::IJob(
    KDevelop::ILaunchConfiguration* launchConfig,
    QString tool,
    bool hasView,
    Plugin* plugin,
    QObject* parent)

    : KDevelop::OutputExecuteJob(parent)
    , m_config(launchConfig->config())
    , m_tool(tool)
    , m_hasView(hasView)
    , m_plugin(plugin)
    , m_tcpServerPort(0)
{
    Q_ASSERT(launchConfig);
    Q_ASSERT(m_plugin);

    setProperties(KDevelop::OutputExecuteJob::JobProperty::DisplayStdout);
    setProperties(KDevelop::OutputExecuteJob::JobProperty::DisplayStderr);
    setProperties(KDevelop::OutputExecuteJob::JobProperty::PostProcessOutput);

    setCapabilities(KJob::Killable);
    setStandardToolView(KDevelop::IOutputView::TestView);
    setBehaviours(KDevelop::IOutputView::AutoScroll);

    auto pluginController = KDevelop::ICore::self()->pluginController();
    auto iface = pluginController->pluginForExtension(QStringLiteral("org.kdevelop.IExecutePlugin"))->extension<IExecutePlugin>();
    Q_ASSERT(iface);

    QString envProfile = iface->environmentProfileName(launchConfig);
    if (envProfile.isEmpty()) {
        envProfile = KDevelop::EnvironmentProfileList(KSharedConfig::openConfig()).defaultProfileName();
    }
    setEnvironmentProfile(envProfile);

    QString errorString;

    m_analyzedExecutable = iface->executable(launchConfig, errorString).toLocalFile();
    if (!errorString.isEmpty()) {
        setError(-1);
        setErrorText(errorString);
    }

    m_analyzedExecutableArguments = iface->arguments(launchConfig, errorString);
    if (!errorString.isEmpty()) {
        setError(-1);
        setErrorText(errorString);
    }

    QUrl workDir = iface->workingDirectory(launchConfig);
    if (workDir.isEmpty() || !workDir.isValid()) {
        workDir = QUrl::fromLocalFile(QFileInfo(m_analyzedExecutable).absolutePath());
    }
    setWorkingDirectory(workDir);

    connect(this, &IJob::finished, m_plugin, &Plugin::jobFinished);

    auto tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::LocalHost);
    m_tcpServerPort = tcpServer->serverPort();

    connect(tcpServer, &QTcpServer::newConnection, this, [this, tcpServer]() {
        auto tcpSocket = tcpServer->nextPendingConnection();

        connect(tcpSocket, &QTcpSocket::readyRead, this, [this, tcpSocket]() {
            QStringList lines;
            while (!tcpSocket->atEnd()) {
                lines += tcpSocket->readLine().trimmed();
            }
            processValgrindOutput(lines);
        });
    });
}

IJob::~IJob()
{
}

QString IJob::tool() const
{
    return m_tool;
}

QString IJob::target() const
{
    return QFileInfo(m_analyzedExecutable).fileName();
}

bool IJob::hasView()
{
    return m_hasView;
}

void IJob::addLoggingArgs(QStringList& args) const
{
    args += QStringLiteral("--log-socket=127.0.0.1:%1").arg(m_tcpServerPort);
}

QStringList IJob::buildCommandLine() const
{
    Generic::Settings settings;
    settings.load(m_config);

    QStringList args;

    args += QStringLiteral("--tool=") + m_tool;
    addLoggingArgs(args);

    args += settings.cmdArgs();
    addToolArgs(args);

    return args;
}

void IJob::start()
{
    *this << Generic::Settings::valgrindExecutablePath();
    *this << buildCommandLine();
    *this << m_analyzedExecutable;
    *this << m_analyzedExecutableArguments;

    qCDebug(KDEV_VALGRIND) << "executing:" << commandLine().join(' ');

    m_plugin->jobReadyToStart(this);
    KDevelop::OutputExecuteJob::start();
}

void IJob::postProcessStderr(const QStringList& lines)
{
    for (const QString& line : lines) {
        if (line.startsWith(valgrindErrorsPrefix)) {
            m_valgrindOutput += line;
        }
    }
    KDevelop::OutputExecuteJob::postProcessStderr(lines);
}

void IJob::processValgrindOutput(const QStringList& lines)
{
    m_valgrindOutput += lines;

    if (GlobalSettings::showValgrindOutput()) {
        KDevelop::OutputExecuteJob::postProcessStderr(lines);
    }
}

void IJob::childProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
{
    qCDebug(KDEV_VALGRIND) << "Process Finished, exitCode" << exitCode << "process exit status" << exitStatus;

    bool ok = !exitCode;
    if (ok) {
        ok = processEnded();
    }

    m_plugin->jobReadyToFinish(this, ok);
    KDevelop::OutputExecuteJob::childProcessExited(exitCode, exitStatus);
}

void IJob::childProcessError(QProcess::ProcessError processError)
{
    QString errorMessage;

    switch (processError) {

    case QProcess::FailedToStart:
        errorMessage = i18n("Failed to start valgrind from \"%1\".", commandLine().first());
        break;

    case QProcess::Crashed:
        // if the process was killed by the user, the crash was expected
        // don't notify the user
        if (status() != KDevelop::OutputExecuteJob::JobStatus::JobCanceled) {
            errorMessage = i18n("Valgrind crashed.");
        }
        break;

    case QProcess::Timedout:
        errorMessage = i18n("Valgrind process timed out.");
        break;

    case QProcess::WriteError:
        errorMessage = i18n("Write to Valgrind process failed.");
        break;

    case QProcess::ReadError:
        errorMessage = i18n("Read from Valgrind process failed.");
        break;

    case QProcess::UnknownError:
        // Here, check if Valgrind failed (because of bad parameters or whatever).
        // Because Valgrind always returns 1 on failure, and the profiled application's return
        // on success, we cannot know for sure which process returned != 0.
        //
        // The only way to guess that it is Valgrind which failed is to check stderr and look for
        // "valgrind: " at the beginning of the first line, even though it can still be the
        // profiled process that writes it on stderr. It is, however, unlikely enough to be
        // reliable in most cases.

        if (!m_valgrindOutput.isEmpty() &&
             m_valgrindOutput.at(0).startsWith(valgrindErrorsPrefix)) {

            errorMessage  = m_valgrindOutput.join('\n').remove(valgrindErrorsPrefix);
            errorMessage += QStringLiteral("\n\n");
            errorMessage += i18n("Please review your Valgrind launch configuration.");
        } else {
            errorMessage = i18n("Unknown Valgrind process error.");
        }
        break;
    }

    if (!errorMessage.isEmpty()) {
        KMessageBox::error(qApp->activeWindow(), errorMessage, i18n("Valgrind Error"));
    }

    KDevelop::OutputExecuteJob::childProcessError(processError);
}

bool IJob::processEnded()
{
    return true;
}

int IJob::executeProcess(const QString& executable, const QStringList& args, QByteArray& processOutput)
{
    QString commandLine = executable + " " + args.join(' ');

    if (GlobalSettings::showValgrindOutput()) {
        KDevelop::OutputExecuteJob::postProcessStdout({i18n("Executing command: ") + commandLine });
    }

    QProcess process;
    process.start(executable, args);
    if (!process.waitForFinished()){
        return -1;
    }

    processOutput = process.readAllStandardOutput();
    QString errOutput(process.readAllStandardError());

    if (GlobalSettings::showValgrindOutput()) {
        KDevelop::OutputExecuteJob::postProcessStdout(QString(processOutput).split('\n'));
    }
    KDevelop::OutputExecuteJob::postProcessStderr(errOutput.split('\n'));

    if (process.exitCode()) {
        QString message = i18n("Failed to execute the command:");
        message += "\n\n";
        message += commandLine;
        message += "\n\n";
        message += i18n("Please review your Valgrind launch configuration.");

        KMessageBox::error(qApp->activeWindow(), message, i18n("Valgrind Error"));
    }

    return process.exitCode();
}

}
