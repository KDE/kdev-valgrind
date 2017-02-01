/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2002 Harald Fernengel <harry@kdevelop.org>
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

#include "ijob.h"

#include "imodel.h"
#include "iview.h"

#include "debug.h"
#include "plugin.h"

#include "globalsettings.h"

#include "cachegrind/job.h"
#include "callgrind/job.h"
#include "generic/settings.h"
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

IJob* IJob::createToolJob(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent)
{
    const QString& toolName = valgrindTools.at(GenericSettings::currentTool(cfg->config()));

    if (toolName == QStringLiteral("memcheck"))
        return new MemcheckJob(cfg, plugin, parent);

    else if (toolName == QStringLiteral("massif"))
        return new MassifJob(cfg, plugin, parent);

    else if (toolName == QStringLiteral("cachegrind"))
        return new CachegrindJob(cfg, plugin, parent);

    else if (toolName == QStringLiteral("callgrind"))
        return new CallgrindJob(cfg, plugin, parent);

    qCDebug(KDEV_VALGRIND) << "can't create this job, " << toolName << " unknow job";

    return nullptr;
}

IJob::IJob(
    KDevelop::ILaunchConfiguration* cfg,
    QString tool,
    IModel* model,
    Plugin* plugin,
    QObject* parent)

    : KDevelop::OutputExecuteJob(parent)
    , m_launchcfg(cfg)
    , m_tool(tool)
    , m_model(model)
    , m_plugin(plugin)
{
    Q_ASSERT(m_launchcfg);
    Q_ASSERT(m_plugin);

    setProperties(KDevelop::OutputExecuteJob::JobProperty::DisplayStdout);
    setProperties(KDevelop::OutputExecuteJob::JobProperty::DisplayStderr);
    setProperties(KDevelop::OutputExecuteJob::JobProperty::PostProcessOutput);

    setCapabilities(KJob::Killable);
    setStandardToolView(KDevelop::IOutputView::TestView);
    setBehaviours(KDevelop::IOutputView::AutoScroll);

    KConfigGroup config(m_launchcfg->config());

    IExecutePlugin* iface = KDevelop::ICore::self()->pluginController()->pluginForExtension("org.kdevelop.IExecutePlugin")->extension<IExecutePlugin>();
    Q_ASSERT(iface);

    KDevelop::EnvironmentGroupList l(KSharedConfig::openConfig());
    QString envgrp = iface->environmentGroup(m_launchcfg);

    if (envgrp.isEmpty()) {
        qCWarning(KDEV_VALGRIND) << i18n("No environment group specified, looks like a broken "
                           "configuration, please check run configuration '%1'. "
                           "Using default environment group.", m_launchcfg->name());
        envgrp = l.defaultGroup();
    }
    // FIXME
//     m_process->setEnvironment(l.createEnvironment(envgrp, m_process->systemEnvironment()));

    QString errorString;

    m_valgrindExecutable = KDevelop::Path(GlobalSettings::valgrindExecutablePath()).toLocalFile();

    m_analyzedExecutable = iface->executable(m_launchcfg, errorString).toLocalFile();
    if (!errorString.isEmpty()) {
        setError(-1);
        setErrorText(errorString);
    }

    m_analyzedExecutableArguments = iface->arguments(m_launchcfg, errorString);
    if (!errorString.isEmpty()) {
        setError(-1);
        setErrorText(errorString);
    }

    m_workingDir = iface->workingDirectory(m_launchcfg);
    if (m_workingDir.isEmpty() || !m_workingDir.isValid())
        m_workingDir = QUrl::fromLocalFile(QFileInfo(m_analyzedExecutable).absolutePath());
//     setWorkingDirectory(m_workingDir.toLocalFile()); // FIXME

    if (m_model)
        m_model->reset();
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

QString IJob::argValue(bool value) const
{
    return value ? QStringLiteral("yes") : QStringLiteral("no");
}

QString IJob::argValue(int value) const
{
    return QString::number(value);
}

QStringList IJob::buildCommandLine() const
{
    KConfigGroup config = m_launchcfg->config();
    QStringList args;

    args += QStringLiteral("--tool=") + m_tool;
    args += KShell::splitArgs(GenericSettings::extraParameters(config));

    args += QStringLiteral("--num-callers=") + argValue(GenericSettings::stackframeDepth(config));
    args += QStringLiteral("--max-stackframe=") + argValue(GenericSettings::maximumStackframeSize(config));
    args += QStringLiteral("--error-limit=") + argValue(GenericSettings::limitErrors(config));

    addToolArgs(args, config);
    args.removeAll(QStringLiteral(""));

    return args;
}

void IJob::start()
{
    if (error()) {
        emitResult();
        return;
    }

    *this << m_valgrindExecutable;
    *this << buildCommandLine();
    *this << m_analyzedExecutable;
    *this << m_analyzedExecutableArguments;

   qCDebug(KDEV_VALGRIND) << "executing:" << commandLine().join(' ');

    beforeStart();
    KDevelop::OutputExecuteJob::start();
    processStarted();
}

void IJob::postProcessStdout(const QStringList& lines)
{
    m_standardOutput << lines;
    KDevelop::OutputExecuteJob::postProcessStdout(lines);
}

void IJob::postProcessStderr(const QStringList& lines)
{
    m_errorOutput << lines;
    KDevelop::OutputExecuteJob::postProcessStderr(lines);
}

void IJob::childProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
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

    processEnded();
    emitResult();

    IView* view = createView();
    if (view) {
        view->setModel(m_model);
        emit m_plugin->addView(view, QStringLiteral("%1 (%2)").arg(target()).arg(tool()));
    }

    m_plugin->jobFinished(this);
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
        if (status() != KDevelop::OutputExecuteJob::JobStatus::JobCanceled)
            errorMessage = i18n("Valgrind crashed.");
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

    if (!errorMessage.isEmpty())
        KMessageBox::error(qApp->activeWindow(), errorMessage, i18n("Valgrind Error"));

    KDevelop::OutputExecuteJob::childProcessError(processError);
}

void IJob::beforeStart()
{
}

void IJob::processStarted()
{
}

void IJob::processEnded()
{
}

int IJob::executeProcess(const QString& executable, const QStringList& args, QByteArray& processOutput)
{
    QProcess process;
    process.start(executable, args);
    if (process.waitForFinished())
        processOutput = process.readAllStandardOutput();

    return process.exitCode();
}

/**
 * QFileProxyRemove Implementation
 */
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

}
