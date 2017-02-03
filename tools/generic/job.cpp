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

#include "job.h"

#include "debug.h"
#include "plugin.h"
#include "settings.h"

#include "cachegrind/job.h"
#include "callgrind/job.h"
#include "massif/job.h"
#include "memcheck/job.h"

#include <execute/iexecuteplugin.h>
#include <interfaces/icore.h>
#include <interfaces/ilaunchconfiguration.h>
#include <interfaces/iplugincontroller.h>
#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <kshell.h>
#include <util/environmentgrouplist.h>

#include <QApplication>
#include <QBuffer>
#include <QFileInfo>

namespace valgrind
{

GenericJob* GenericJob::createToolJob(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent)
{
    const QString& toolName = valgrindTools.at(GenericSettings(cfg->config()).currentTool());

    if (toolName == QStringLiteral("memcheck")) {
        return new MemcheckJob(cfg, plugin, parent);
    }

    else if (toolName == QStringLiteral("massif")) {
        return new MassifJob(cfg, plugin, parent);
    }

    else if (toolName == QStringLiteral("cachegrind")) {
        return new CachegrindJob(cfg, plugin, parent);
    }

    else if (toolName == QStringLiteral("callgrind")) {
        return new CallgrindJob(cfg, plugin, parent);
    }

    qCDebug(KDEV_VALGRIND) << "can't create this job, " << toolName << " unknow job";

    return nullptr;
}

GenericJob::GenericJob(
    KDevelop::ILaunchConfiguration* launchConfig,
    QString tool,
    Plugin* plugin,
    QObject* parent)

    : KDevelop::OutputExecuteJob(parent)
    , config(launchConfig->config())
    , m_tool(tool)
    , m_plugin(plugin)
{
    Q_ASSERT(launchConfig);
    Q_ASSERT(m_plugin);

    setProperties(KDevelop::OutputExecuteJob::JobProperty::DisplayStdout);
    setProperties(KDevelop::OutputExecuteJob::JobProperty::DisplayStderr);
    setProperties(KDevelop::OutputExecuteJob::JobProperty::PostProcessOutput);

    setCapabilities(KJob::Killable);
    setStandardToolView(KDevelop::IOutputView::TestView);
    setBehaviours(KDevelop::IOutputView::AutoScroll);

    IExecutePlugin* iface = KDevelop::ICore::self()->pluginController()->pluginForExtension("org.kdevelop.IExecutePlugin")->extension<IExecutePlugin>();
    Q_ASSERT(iface);

    KDevelop::EnvironmentGroupList envGroupList(KSharedConfig::openConfig());
    QString envGroup = iface->environmentGroup(launchConfig);

    if (envGroup.isEmpty()) {
        qCWarning(KDEV_VALGRIND) << i18n("No environment group specified, looks like a broken "
                           "configuration, please check run configuration '%1'. "
                           "Using default environment group.", launchConfig->name());
        envGroup = envGroupList.defaultGroup();
    }
    // FIXME
//     m_process->setEnvironment(l.createEnvironment(envgrp, m_process->systemEnvironment()));

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

    m_workingDir = iface->workingDirectory(launchConfig);
    if (m_workingDir.isEmpty() || !m_workingDir.isValid()) {
        m_workingDir = QUrl::fromLocalFile(QFileInfo(m_analyzedExecutable).absolutePath());
    }
//     setWorkingDirectory(m_workingDir.toLocalFile()); // FIXME
}

GenericJob::~GenericJob()
{
}

QString GenericJob::tool() const
{
    return m_tool;
}

QString GenericJob::target() const
{
    return QFileInfo(m_analyzedExecutable).fileName();
}

QStringList GenericJob::argValue(const QString& line) const
{
    return KShell::splitArgs(line);
}

QString GenericJob::argValue(bool value) const
{
    return value ? QStringLiteral("yes") : QStringLiteral("no");
}

QString GenericJob::argValue(int value) const
{
    return QString::number(value);
}

QStringList GenericJob::buildCommandLine() const
{
    GenericSettings settings(config);
    QStringList args;

    args += QStringLiteral("--tool=") + m_tool;
    args += argValue(settings.extraParameters());
    args += QStringLiteral("--num-callers=") + argValue(settings.stackframeDepth());
    args += QStringLiteral("--max-stackframe=") + argValue(settings.maximumStackframeSize());
    args += QStringLiteral("--error-limit=") + argValue(settings.limitErrors());

    addToolArgs(args);

    return args;
}

void GenericJob::start()
{
    if (error()) {
        emitResult();
        return;
    }

    *this << GenericSettings(config).valgrindExecutablePath();
    *this << buildCommandLine();
    *this << m_analyzedExecutable;
    *this << m_analyzedExecutableArguments;

   qCDebug(KDEV_VALGRIND) << "executing:" << commandLine().join(' ');

    KDevelop::OutputExecuteJob::start();
}

void GenericJob::postProcessStdout(const QStringList& lines)
{
    m_standardOutput << lines;
    KDevelop::OutputExecuteJob::postProcessStdout(lines);
}

void GenericJob::postProcessStderr(const QStringList& lines)
{
    m_errorOutput << lines;
    KDevelop::OutputExecuteJob::postProcessStderr(lines);
}

void GenericJob::childProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
{
    qCDebug(KDEV_VALGRIND) << "Process Finished, exitCode" << exitCode << "process exit status" << exitStatus;

    if (exitCode) {
        /*
        ** Here, check if Valgrind failed (because of bad parameters or whatever).
        ** Because Valgrind always returns 1 on failure, and the profiled application's return
        ** on success, we cannot know for sure which process returned != 0.
        **
        ** The only way to guess that it is Valgrind which failed is to check stderr and look for
        ** "valgrind:" at the beginning of each line, even though it can still be the profiled
        ** process that writes it on stderr. It is, however, unlikely enough to be reliable in
        ** most cases.
        */
        const QString s = m_errorOutput.join(' ');

        if (s.startsWith("valgrind:")) {
            QString err = s.split("\n")[0];
            err = err.replace("valgrind:", "");
            err += "\n\nPlease review your Valgrind launch configuration.";

            KMessageBox::error(qApp->activeWindow(), err, i18n("Valgrind Error"));
        }
    }

    if (processEnded()) {
        if (QWidget* view = createView()) {
            emit m_plugin->addView(view, QStringLiteral("%1 (%2)").arg(target()).arg(tool()));
        }

        m_plugin->jobFinished(this);
    }

    emitResult();
}

void GenericJob::childProcessError(QProcess::ProcessError processError)
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
        errorMessage = i18n("Unknown Valgrind process error.");
        break;
    }

    if (!errorMessage.isEmpty()) {
        KMessageBox::error(qApp->activeWindow(), errorMessage, i18n("Valgrind Error"));
    }

    KDevelop::OutputExecuteJob::childProcessError(processError);
}

bool GenericJob::processEnded()
{
    return true;
}

int GenericJob::executeProcess(const QString& executable, const QStringList& args, QByteArray& processOutput)
{
    QString commandLine("Executing command: ");
    commandLine += executable + " ";
    commandLine += args.join(' ');
    KDevelop::OutputExecuteJob::postProcessStdout({"", commandLine });

    QProcess process;
    process.start(executable, args);
    if (!process.waitForFinished()){
        return -1;
    }

    processOutput = process.readAllStandardOutput();
    QString errOutput(process.readAllStandardError());

    KDevelop::OutputExecuteJob::postProcessStdout(QString(processOutput).split('\n'));
    KDevelop::OutputExecuteJob::postProcessStderr(errOutput.split('\n'));

    return process.exitCode();
}

}
