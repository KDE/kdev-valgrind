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

#include "job.h"

#include "debug.h"

#include <QApplication>
#include <QBuffer>
#include <QFileInfo>

#include <kconfiggroup.h>
#include "debug.h"
#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <KProcess>
#include <kshell.h>

#include <util/processlinemaker.h>
#include <outputview/outputmodel.h>
#include <util/environmentgrouplist.h>
#include <interfaces/ilaunchconfiguration.h>
#include <interfaces/icore.h>
#include <interfaces/iplugincontroller.h>

#include <execute/iexecuteplugin.h>
#include <assert.h>

#include "memcheckfakemodel.h"
#include "memcheckparser.h"
#include "memcheckjob.h"

#include "massifmodel.h"
#include "massifparser.h"
#include "massifjob.h"

#include "cachegrindjob.h"
#include "cachegrindmodel.h"
#include "cachegrindparser.h"

#include "callgrindmodel.h"
#include "callgrindparser.h"
#include "callgrindjob.h"

#include "plugin.h"
#include "modelwrapper.h"

namespace valgrind
{

/*!
 * Creates a model and a parser according to the specified name and
 * connects the 2 items
 */
class ModelParserFactoryPrivate
{
public:
    void make(const QString& type, Model*& m_model, Parser*& m_parser);
};

void ModelParserFactoryPrivate::make(const QString& tool, Model*& m_model, Parser*& m_parser)
{
    ModelWrapper* modelWrapper = nullptr;

    if (tool == QStringLiteral("memcheck")) {
        m_model = new MemcheckFakeModel();
        modelWrapper = new ModelWrapper(m_model);
        m_parser = new MemcheckParser();

        // FIXME replace with new style
        QObject::connect(m_parser, SIGNAL(newElement(Model::eElementType)),
                         modelWrapper, SLOT(newElement(Model::eElementType)));
        QObject::connect(m_parser, SIGNAL(newData(Model::eElementType, QString, QString)),
                         modelWrapper, SLOT(newData(Model::eElementType, QString, QString)));
    }

    else if (tool == QStringLiteral("massif")) {
        m_model = new MassifModel();
        modelWrapper = new ModelWrapper(m_model);
        m_parser = new MassifParser();

        // FIXME replace with new style
        QObject::connect(m_parser, SIGNAL(newItem(ModelItem*)),
                         modelWrapper, SLOT(newItem(ModelItem*)));
    }

    else if (tool == QStringLiteral("callgrind")) {
        m_model = new CallgrindModel();
        modelWrapper = new ModelWrapper(m_model);
        m_parser = new CallgrindParser();

        // FIXME replace with new style
        QObject::connect(m_parser, SIGNAL(newItem(ModelItem*)),
                         modelWrapper, SLOT(newItem(ModelItem*)));
    }

    else if (tool == QStringLiteral("cachegrind")) {
        m_model = new CachegrindModel();
        modelWrapper = new ModelWrapper(m_model);
        m_parser = new CachegrindParser();

        // FIXME replace with new style
        QObject::connect(m_parser, SIGNAL(newItem(ModelItem*)),
                         modelWrapper, SLOT(newItem(ModelItem*)));
    }

    m_model->setModelWrapper(modelWrapper);
    QObject::connect(m_parser, SIGNAL(reset()), modelWrapper, SLOT(reset()));
    m_model->reset();
}

// The factory for jobs
Job* Job::createToolJob(KDevelop::ILaunchConfiguration* cfg, Plugin* inst, QObject* parent)
{
    const QString& name = cfg->config().readEntry(QStringLiteral("Current Tool"), QStringLiteral("memcheck"));

    if (name == QStringLiteral("memcheck"))
        return new MemcheckJob(cfg, inst, parent);

    else if (name == QStringLiteral("massif"))
        return new MassifJob(cfg, inst, parent);

    else if (name == QStringLiteral("cachegrind"))
        return new CachegrindJob(cfg, inst, parent);

    else if (name == QStringLiteral("callgrind"))
        return new CallgrindJob(cfg, inst, parent);

    qCDebug(KDEV_VALGRIND) << "can't create this job, " << name << "unknow job";

    return nullptr;
}

Job::Job(KDevelop::ILaunchConfiguration* cfg, Plugin* inst, QObject* parent)
    : KDevelop::OutputJob(parent)
    , m_process(new KProcess(this))
    , m_pid(0)
    , m_model(nullptr)
    , m_parser(nullptr)
    , m_applicationOutput(new KDevelop::ProcessLineMaker(this))
    , m_launchcfg(cfg)
    , m_plugin(inst)
    , m_killed(false)
{
    setCapabilities(KJob::Killable);
    m_process->setOutputChannelMode(KProcess::SeparateChannels);

    // FIXME replace with new style
    connect(m_process,  SIGNAL(readyReadStandardOutput()),
            SLOT(readyReadStandardOutput()));
    connect(m_process,  SIGNAL(readyReadStandardError()),
            SLOT(readyReadStandardError()));
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
            SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            SLOT(processErrored(QProcess::ProcessError)));

    // create the correct model for each tool
    QString tool = m_launchcfg->config().readEntry(QStringLiteral("Current Tool"), QStringLiteral("memcheck"));
    ModelParserFactoryPrivate factory;

    factory.make(tool, m_model, m_parser);
    m_model->getModelWrapper()->job(this);
    m_plugin->incomingModel(m_model);

    connect(this, &Job::finished, inst, &Plugin::jobFinished);
}

Job::~Job()
{
    doKill();
    if (m_model && m_model->getModelWrapper())
        m_model->getModelWrapper()->job(nullptr);

    delete m_process;
    delete m_applicationOutput;
    delete m_parser;
}

void Job::processModeArgs(
    QStringList& out,
    const t_valgrind_cfg_argarray mode_args,
    int mode_args_count,
    KConfigGroup& cfg) const
{
    // For each option, set the right string in the arguments list
    for (int i = 0; i < mode_args_count; ++i) {
        QString val;
        QString argtype = mode_args[i][2];

        if (argtype == QStringLiteral("str"))
            val = cfg.readEntry(mode_args[i][0]);

        else if (argtype == QStringLiteral("int")) {
            int n = cfg.readEntry(mode_args[i][0], 0);
            if (n)
                val.sprintf("%d", n);
        }

        else if (argtype == QStringLiteral("bool")) {
            bool n = cfg.readEntry(mode_args[i][0], false);
            val = n ? QStringLiteral("yes") : QStringLiteral("no");
        }

        else if (argtype == QStringLiteral("float")) {
            int n = cfg.readEntry(mode_args[i][0], 1);
            val.sprintf("%d.0", n);
        }

        else if (argtype == QStringLiteral("float")) {
            int n = cfg.readEntry(mode_args[i][0], 1);
            val.sprintf("%d.0", n);
        }

        if (!val.isEmpty())
            out << QString(mode_args[i][1]) + val;
    }
}

void Job::readyReadStandardError()
{
    m_applicationOutput->slotReceivedStderr(m_process->readAllStandardError());
}

void Job::readyReadStandardOutput()
{
    m_applicationOutput->slotReceivedStdout(m_process->readAllStandardOutput());
}

QStringList Job::buildCommandLine() const
{
    static const t_valgrind_cfg_argarray generic_args = {
        {QStringLiteral("Current Tool"), QStringLiteral("--tool="), QStringLiteral("str")},
        {QStringLiteral("Stackframe Depth"), QStringLiteral("--num-callers="), QStringLiteral("int")},
        {QStringLiteral("Maximum Stackframe Size"), QStringLiteral("--max-stackframe="), QStringLiteral("int")},
        {QStringLiteral("Limit Errors"), QStringLiteral("--error-limit="), QStringLiteral("bool")}
    };

    static const int generic_args_count = sizeof(generic_args) / sizeof(*generic_args);

    KConfigGroup cfg = m_launchcfg->config();
    QString tool = cfg.readEntry(QStringLiteral("Current Tool"), QStringLiteral("memcheck"));

    QStringList args = KShell::splitArgs(cfg.readEntry(QStringLiteral("Valgrind Arguments"), QStringLiteral("")));
    processModeArgs(args, generic_args, generic_args_count, cfg);

    addToolArgs(args, cfg);

    return args;
}

void Job::start()
{
    KConfigGroup grp = m_launchcfg->config();
    IExecutePlugin* iface = KDevelop::ICore::self()->pluginController()->pluginForExtension("org.kdevelop.IExecutePlugin")->extension<IExecutePlugin>();
    KDevelop::EnvironmentGroupList l(KSharedConfig::openConfig());
    QString envgrp = iface->environmentGroup(m_launchcfg);
    QString err;
    QString executable = iface->executable(m_launchcfg, err).toLocalFile();

    Q_ASSERT(iface);
    if (!err.isEmpty()) {
        setError(-1);
        setErrorText(err);
        emit updateTabText(m_model, i18n("job failed"));

        return;
    }

    if (envgrp.isEmpty()) {
        qCWarning(KDEV_VALGRIND) << i18n("No environment group specified, looks like a broken "
                           "configuration, please check run configuration '%1'. "
                           "Using default environment group.", m_launchcfg->name());
        envgrp = l.defaultGroup();
    }

    QStringList arguments = iface->arguments(m_launchcfg, err);

    if (!err.isEmpty()) {
        setError(-1);
        setErrorText(err);
        emit updateTabText(m_model, i18n("job failed"));
    }

    if (error()) {
        emitResult();

        return;
    }

    setStandardToolView(KDevelop::IOutputView::DebugView);
    setBehaviours(KDevelop::IOutputView::AllowUserClose | KDevelop::IOutputView::AutoScroll);
    setModel(new KDevelop::OutputModel());

    m_process->setEnvironment(l.createEnvironment(envgrp, m_process->systemEnvironment()));

    Q_ASSERT(m_process->state() != QProcess::Running);

    m_workingDir = iface->workingDirectory(m_launchcfg);
    if (m_workingDir.isEmpty() || !m_workingDir.isValid())
        m_workingDir = QUrl::fromLocalFile(QFileInfo(executable).absolutePath());

    m_process->setWorkingDirectory(m_workingDir.toLocalFile());
    m_process->setProperty("executable", executable);

    // some tools need to initialize stuff before the process starts
    this->beforeStart();

    QStringList valgrindArgs;

    valgrindArgs = buildCommandLine();
    valgrindArgs << executable;
    valgrindArgs += arguments;

    QString e = grp.readEntry("Valgrind Executable", QString("/usr/bin/valgrind"));

    qCDebug(KDEV_VALGRIND) << "executing:" << e << valgrindArgs;

    m_process->setProgram(e,valgrindArgs);
    m_process->start();
    m_pid = m_process->pid();

    setTitle(QString(i18n("job output (pid=%1)", m_pid)));
    startOutput();

    // FIXME replace with new style
    connect(m_applicationOutput, SIGNAL(receivedStdoutLines(QStringList)),
            model(), SLOT(appendLines(QStringList)));
    connect(m_applicationOutput, SIGNAL(receivedStderrLines(QStringList)),
            model(), SLOT(appendLines(QStringList)));

    emit updateTabText(m_model, i18n("job running (pid=%1)",  m_pid));

    this->processStarted();
}

bool Job::doKill()
{
    if (m_process && m_process->pid()) {
        m_process->kill();
        m_killed = true;
        m_process = nullptr;
    }

    return true;
}

Plugin* Job::plugin() const
{
    return m_plugin;
}

void Job::processErrored(QProcess::ProcessError e)
{
    switch (e) {

    case QProcess::FailedToStart:
        KMessageBox::error(qApp->activeWindow(), i18n("Failed to start valgrind from \"%1\".", m_process->property("executable").toString()), i18n("Failed to start Valgrind"));
        break;

    case QProcess::Crashed:
        // if the process was killed by the user, the crash was expected
        // don't notify the user
        if (!m_killed)
            KMessageBox::error(qApp->activeWindow(), i18n("Valgrind crashed."), i18n("Valgrind Error"));
        break;

    case QProcess::Timedout:
        KMessageBox::error(qApp->activeWindow(), i18n("Valgrind process timed out."), i18n("Valgrind Error"));
        break;

    case QProcess::WriteError:
        KMessageBox::error(qApp->activeWindow(), i18n("Write to Valgrind process failed."), i18n("Valgrind Error"));
        break;

    case QProcess::ReadError:
        KMessageBox::error(qApp->activeWindow(), i18n("Read from Valgrind process failed."), i18n("Valgrind Error"));
        break;

    case QProcess::UnknownError:
        KMessageBox::error(qApp->activeWindow(), i18n("Unknown Valgrind process error."), i18n("Valgrind Error"));
        break;
    }
}

void Job::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qCDebug(KDEV_VALGRIND) << "Process Finished, exitCode" << exitCode << "process exit status" << exitStatus;

    QString tabname = i18n("job finished (pid=%1,exit=%2)", m_pid, exitCode);

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
        const QString &s = m_process->readAllStandardError();

        if (s.startsWith("valgrind:")) {
            QString err = s.split("\n")[0];
            err = err.replace("valgrind:", "");
            err += "\n\nPlease review your Valgrind launch configuration.";

            KMessageBox::error(qApp->activeWindow(), err, i18n("Valgrind Error"));
            tabname = i18n("job failed");
        }
    }

    this->processEnded();
    emit updateTabText(m_model, tabname);
    emitResult();
}

void Job::beforeStart()
{
}

void Job::processStarted()
{
}

void Job::processEnded()
{
}

KDevelop::OutputModel* Job::model()
{
    return dynamic_cast<KDevelop::OutputModel*>(KDevelop::OutputJob::model());
}

/**
 * KProcessOutputToParser implementation
 */
KProcessOutputToParser::KProcessOutputToParser(Parser* parser)
{
    m_parser = parser;
    m_device = new QBuffer();
    m_device->open(QIODevice::WriteOnly);
}

KProcessOutputToParser::~KProcessOutputToParser()
{
    if (m_device)
        delete m_device;

    if (m_process)
        delete m_process;
}

int KProcessOutputToParser::execute(const QString& execPath, const QStringList& args)
{
    m_process = new KProcess();
    m_process->setOutputChannelMode(KProcess::OnlyStdoutChannel);
    m_process->setProgram(execPath, args);

    // FIXME replace with new style
    QObject::connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(newDataFromStdOut()));
    QObject::connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processEnded(int, QProcess::ExitStatus)));

    // execute and wait the end of the program
    return m_process->execute();
}

void KProcessOutputToParser::processEnded(int , QProcess::ExitStatus status)
{
    qCDebug(KDEV_VALGRIND) << status;

    if (status == QProcess::NormalExit) {
        m_device->close();
        m_device->open(QIODevice::ReadOnly);
        m_parser->setDevice(m_device);
        m_parser->parse();
    }
}

void KProcessOutputToParser::newDataFromStdOut()
{
    m_device->write(m_process->readAllStandardOutput());
}

/**
 * QFileProxyRemove Implementation
 */
QFileProxyRemove::QFileProxyRemove(const QString& programPath, const QStringList& args, QFile *toRemove, QObject *parent)
    : QObject(parent)
{
    m_execPath = programPath;
    m_process = new KProcess();
    m_file = toRemove;
    m_process->setProgram(programPath, args);

    // FIXME replace with new style
    QObject::connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
                     this, SLOT(prEnded(int, QProcess::ExitStatus)));
    QObject::connect(m_process, SIGNAL(error(QProcess::ProcessError)),
                     this, SLOT(prError(QProcess::ProcessError)));

    m_process->start();
}

QFileProxyRemove::~QFileProxyRemove()
{
    if (m_file)
        m_file->remove();

    delete m_process;
}

void QFileProxyRemove::prError(QProcess::ProcessError error)
{
    if (error == QProcess::FailedToStart)
        KMessageBox::error(qApp->activeWindow(), i18n("Unable to launch the process %1 (%2)", m_execPath, error), i18n("Valgrind Error"));
}

void QFileProxyRemove::prEnded(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(status)

    m_file->remove();
    delete m_file;
    m_file = nullptr;
}

}
