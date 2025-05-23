/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2002 Harald Fernengel <harry@kdevelop.org>
   Copyright 2016-2017 Anton Anikin <anton@anikin.xyz>

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
#include "globalsettings.h"
#include "plugin.h"
#include "tool.h"
#include "utils.h"
#include "private/common_config.h"

#include <execute/iexecuteplugin.h>
#include <interfaces/icore.h>
#include <interfaces/ilaunchconfiguration.h>
#include <interfaces/iruncontroller.h>
#include <interfaces/iuicontroller.h>
#include <util/environmentprofilelist.h>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KShell>

#include <QBuffer>
#include <QFileInfo>
#include <QTcpServer>
#include <QTcpSocket>

namespace Valgrind
{

inline QString valgrindErrorsPrefix() { return QStringLiteral("valgrind: "); }

Job::Job(const Tool* tool, const LaunchInfo& launchInfo)
    : KDevelop::OutputExecuteJob(KDevelop::ICore::self()->runController())
    , m_tool(tool)
    , m_configGroup(launchInfo.launchConfiguration.config())
    , m_tcpServerPort(0)
{
    Q_ASSERT(tool);
    setCapabilities(Killable);

    auto& execute = launchInfo.execute;
    auto& launchConfiguration = launchInfo.launchConfiguration;

    QString errorString;
    const auto detectError = [&errorString, this](int errorCode) {
        if (errorString.isEmpty()) {
            return false;
        }
        setError(errorCode);
        setErrorText(errorString);
        return true;
    };

    m_analyzedExecutable = execute.executable(&launchConfiguration, errorString).toLocalFile();
    if (detectError(-1)) {
        return;
    }
    m_analyzedExecutableArguments = execute.arguments(&launchConfiguration, errorString);
    if (detectError(-2)) {
        return;
    }

    setProperties(KDevelop::OutputExecuteJob::JobProperty::DisplayStdout);
    setProperties(KDevelop::OutputExecuteJob::JobProperty::DisplayStderr);
    setProperties(KDevelop::OutputExecuteJob::JobProperty::PostProcessOutput);

    setStandardToolView(KDevelop::IOutputView::DebugView);
    setBehaviours(KDevelop::IOutputView::AllowUserClose | KDevelop::IOutputView::AutoScroll);
    // Valgrind analysis runs a native program and prints its output along with the output of Valgrind itself
    setFilteringStrategy(KDevelop::OutputModel::NativeAppErrorFilter);

    auto envProfile = execute.environmentProfileName(&launchConfiguration);
    if (envProfile.isEmpty()) {
        envProfile = KDevelop::EnvironmentProfileList(KSharedConfig::openConfig()).defaultProfileName();
    }
    setEnvironmentProfile(envProfile);

    const QFileInfo analyzedExecutableInfo(m_analyzedExecutable);

    {
        const auto analyzerName = m_tool->name();
        const auto targetName = analyzedExecutableInfo.fileName();
        setObjectName(i18n("%1 Analysis (%2)", analyzerName, targetName));
        // shorten the output tab title to show more tabs without scrolling
        setTitle(i18nc("%1 - the name of a Valgrind tool, %2 - the name of the target of a Valgrind analysis",
                       "%1 (%2)", analyzerName, targetName));
    }

    auto workDir = execute.workingDirectory(&launchConfiguration);
    if (workDir.isEmpty() || !workDir.isValid()) {
        workDir = QUrl::fromLocalFile(analyzedExecutableInfo.absolutePath());
    }
    setWorkingDirectory(workDir);

    connect(this, &Job::finished, Plugin::self(), &Plugin::jobFinished);

    auto tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::LocalHost);
    m_tcpServerPort = tcpServer->serverPort();

    connect(tcpServer, &QTcpServer::newConnection, this, [this, tcpServer]() {
        auto tcpSocket = tcpServer->nextPendingConnection();

        connect(tcpSocket, &QTcpSocket::readyRead, this, [this, tcpSocket]() {
            QStringList lines;
            while (!tcpSocket->atEnd()) {
                lines += QString::fromLocal8Bit(tcpSocket->readLine().trimmed());
            }
            processValgrindOutput(lines);
        });
    });

    connect(this, &Job::finished, this, [this]() {
        Q_EMIT hideProgress(this);
    });
    KDevelop::ICore::self()->uiController()->registerStatus(this);
}

const Tool* Job::tool() const
{
    return m_tool;
}

QString Job::statusName() const
{
    return objectName();
}

void Job::addLoggingArgs(QStringList& args) const
{
    args += QStringLiteral("--log-socket=127.0.0.1:%1").arg(m_tcpServerPort);
}

QStringList Job::buildCommandLine() const
{
    CommonConfig config;
    config.setConfigGroup(m_configGroup);
    config.load();

    QStringList args;

    args += QStringLiteral("--tool=%1").arg(m_tool->valgrindToolName());
    addLoggingArgs(args);

    args += config.cmdArgs();
    addToolArgs(args);

    return args;
}

void Job::start()
{
    if (error() != NoError) {
        emitResult();
        return;
    }

    *this << KDevelop::Path(GlobalSettings::valgrindExecutablePath()).toLocalFile();
    *this << buildCommandLine();
    *this << m_analyzedExecutable;
    *this << m_analyzedExecutableArguments;

    qCDebug(KDEV_VALGRIND) << "executing:" << commandLine().join(QLatin1Char(' '));

    Plugin::self()->jobReadyToStart(this);

    Q_EMIT showProgress(this, 0, 0, 0);
    KDevelop::OutputExecuteJob::start();
}

void Job::postProcessStderr(const QStringList& lines)
{
    for (const QString& line : lines) {
        if (line.startsWith(valgrindErrorsPrefix())) {
            m_valgrindOutput += line;
        }
    }
    KDevelop::OutputExecuteJob::postProcessStderr(lines);
}

void Job::processValgrindOutput(const QStringList& lines)
{
    m_valgrindOutput += lines;

    if (GlobalSettings::showValgrindOutput()) {
        KDevelop::OutputExecuteJob::postProcessStderr(lines);
    }
}

void Job::childProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
{
    qCDebug(KDEV_VALGRIND) << "Process Finished, exitCode" << exitCode << "process exit status" << exitStatus;

    bool ok = !exitCode;
    if (ok) {
        ok = processEnded();
    }

    Plugin::self()->jobReadyToFinish(this, ok);
    KDevelop::OutputExecuteJob::childProcessExited(exitCode, exitStatus);
}

void Job::childProcessError(QProcess::ProcessError processError)
{
    QString errorMessage;

    switch (processError) {

    case QProcess::FailedToStart:
        errorMessage = i18n("Failed to start valgrind from \"%1\".", commandLine().at(0));
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
             m_valgrindOutput.at(0).startsWith(valgrindErrorsPrefix())) {

            errorMessage  = m_valgrindOutput.join(QLatin1Char('\n')).remove(valgrindErrorsPrefix());
            errorMessage += QStringLiteral("\n\n");
            errorMessage += i18n("Please review your Valgrind launch configuration.");
        } else {
            errorMessage = i18n("Unknown Valgrind process error.");
        }
        break;
    }

    if (!errorMessage.isEmpty()) {
        KMessageBox::error(activeMainWindow(), errorMessage, i18n("Valgrind Error"));
    }

    KDevelop::OutputExecuteJob::childProcessError(processError);
}

bool Job::processEnded()
{
    return true;
}

int Job::executeProcess(const QString& executable, const QStringList& args, QByteArray& processOutput)
{
    QString commandLine = executable + QLatin1Char(' ') + args.join(QLatin1Char(' '));

    if (GlobalSettings::showValgrindOutput()) {
        KDevelop::OutputExecuteJob::postProcessStdout({i18n("Executing command: ") + commandLine });
    }

    QProcess process;
    process.start(executable, args);
    if (!process.waitForFinished()){
        return -1;
    }

    processOutput = process.readAllStandardOutput();
    QString errOutput = QString::fromLocal8Bit(process.readAllStandardError());

    if (GlobalSettings::showValgrindOutput()) {
        KDevelop::OutputExecuteJob::postProcessStdout(QString::fromLocal8Bit(processOutput).split(QLatin1Char('\n')));
    }
    KDevelop::OutputExecuteJob::postProcessStderr(errOutput.split(QLatin1Char('\n')));

    if (process.exitCode()) {
        QString message = i18n("Failed to execute the command:");
        message += QLatin1String("\n\n");
        message += commandLine;
        message += QLatin1String("\n\n");
        message += i18n("Please review your Valgrind launch configuration.");

        KMessageBox::error(activeMainWindow(), message, i18n("Valgrind Error"));
    }

    return process.exitCode();
}

}
