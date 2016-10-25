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

#include "cachegrindjob.h"
#include "cachegrindmodel.h"
#include "cachegrindparser.h"
#include "callgrindjob.h"
#include "callgrindmodel.h"
#include "callgrindparser.h"
#include "massifjob.h"
#include "massifmodel.h"
#include "massifparser.h"
#include "memcheckfakemodel.h"
#include "memcheckjob.h"
#include "memcheckparser.h"
#include "modelwrapper.h"
#include "plugin.h"

#include <execute/iexecuteplugin.h>
#include <interfaces/icore.h>
#include <interfaces/ilaunchconfiguration.h>
#include <interfaces/iplugincontroller.h>
#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <kshell.h>
#include <util/environmentgrouplist.h>

#include <QApplication>
#include <QBuffer>
#include <QFileInfo>
#include <QRegularExpression>

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

        QObject::connect(m_parser, &Parser::newElement, modelWrapper, &ModelWrapper::newElement);
        QObject::connect(m_parser, &Parser::newData, modelWrapper, &ModelWrapper::newData);
    }

    else if (tool == QStringLiteral("massif")) {
        m_model = new MassifModel();
        modelWrapper = new ModelWrapper(m_model);
        m_parser = new MassifParser();

        QObject::connect(m_parser, &Parser::newItem, modelWrapper, &ModelWrapper::newItem);
    }

    else if (tool == QStringLiteral("callgrind")) {
        m_model = new CallgrindModel();
        modelWrapper = new ModelWrapper(m_model);
        m_parser = new CallgrindParser();

        QObject::connect(m_parser, &Parser::newItem, modelWrapper, &ModelWrapper::newItem);
    }

    else if (tool == QStringLiteral("cachegrind")) {
        m_model = new CachegrindModel();
        modelWrapper = new ModelWrapper(m_model);
        m_parser = new CachegrindParser();

        QObject::connect(m_parser, &Parser::newItem, modelWrapper, &ModelWrapper::newItem);
    }

    m_model->setModelWrapper(modelWrapper);
    QObject::connect(m_parser, &Parser::reset, modelWrapper, &ModelWrapper::reset);
    m_model->reset();
}

// The factory for jobs
Job* Job::createToolJob(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent)
{
    const QString& name = cfg->config().readEntry(QStringLiteral("Current Tool"), QStringLiteral("memcheck"));

    if (name == QStringLiteral("memcheck"))
        return new MemcheckJob(cfg, plugin, parent);

    else if (name == QStringLiteral("massif"))
        return new MassifJob(cfg, plugin, parent);

    else if (name == QStringLiteral("cachegrind"))
        return new CachegrindJob(cfg, plugin, parent);

    else if (name == QStringLiteral("callgrind"))
        return new CallgrindJob(cfg, plugin, parent);

    qCDebug(KDEV_VALGRIND) << "can't create this job, " << name << "unknow job";

    return nullptr;
}

Job::Job(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent)
    : KDevelop::OutputExecuteJob(parent)
    , m_model(nullptr)
    , m_parser(nullptr)
    , m_launchcfg(cfg)
    , m_plugin(plugin)
{
    setProperties(KDevelop::OutputExecuteJob::JobProperty::DisplayStdout);
    setProperties(KDevelop::OutputExecuteJob::JobProperty::DisplayStderr);
    setProperties(KDevelop::OutputExecuteJob::JobProperty::PostProcessOutput);

    setCapabilities(KJob::Killable);
    setStandardToolView(KDevelop::IOutputView::TestView);
    setBehaviours(KDevelop::IOutputView::AutoScroll);

    // create the correct model for each tool
    QString tool = m_launchcfg->config().readEntry(QStringLiteral("Current Tool"), QStringLiteral("memcheck"));
    ModelParserFactoryPrivate factory;

    factory.make(tool, m_model, m_parser);
    m_model->getModelWrapper()->job(this);
    m_plugin->incomingModel(m_model);
}

Job::~Job()
{
    doKill();
    if (m_model && m_model->getModelWrapper())
        m_model->getModelWrapper()->job(nullptr);

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
    }

    if (error()) {
        emitResult();
        return;
    }

    m_workingDir = iface->workingDirectory(m_launchcfg);
    if (m_workingDir.isEmpty() || !m_workingDir.isValid())
        m_workingDir = QUrl::fromLocalFile(QFileInfo(executable).absolutePath());

    // FIXME
//     setWorkingDirectory(m_workingDir.toLocalFile());
//     m_process->setEnvironment(l.createEnvironment(envgrp, m_process->systemEnvironment()));
//     Q_ASSERT(m_process->state() != QProcess::Running);

    // some tools need to initialize stuff before the process starts
    this->beforeStart();

    QStringList valgrindArgs;

    valgrindArgs = buildCommandLine();
    valgrindArgs << executable;
    valgrindArgs += arguments;

    QString e = grp.readEntry("Valgrind Executable", QString("/usr/bin/valgrind"));

    qCDebug(KDEV_VALGRIND) << "executing:" << e << valgrindArgs;

    *this << e;
    *this << valgrindArgs;

    KDevelop::OutputExecuteJob::start();

    this->processStarted();
}

void Job::postProcessStdout(const QStringList& lines)
{
    m_standardOutput << lines;
    KDevelop::OutputExecuteJob::postProcessStdout(lines);
}

void Job::postProcessStderr(const QStringList& lines)
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

void Job::childProcessExited(int exitCode, QProcess::ExitStatus exitStatus)
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

    m_plugin->jobFinished(this);
}

void Job::childProcessError(QProcess::ProcessError processError)
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

void Job::beforeStart()
{
}

void Job::processStarted()
{
}

void Job::processEnded()
{
}

/**
 * KProcessOutputToParser implementation
 */
KProcessOutputToParser::KProcessOutputToParser(Parser* parser)
    : m_process(nullptr)
    , m_device(new QBuffer)
    , m_parser(parser)
{
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
    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::ForwardedErrorChannel);

    connect(m_process, &QProcess::readyReadStandardOutput, this, [this]() {
        m_device->write(m_process->readAllStandardOutput());
    });

    connect(m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, [this](int, QProcess::ExitStatus status) {
        qCDebug(KDEV_VALGRIND) << status;

        if (status == QProcess::NormalExit) {
            m_device->close();
            m_device->open(QIODevice::ReadOnly);
            m_parser->setDevice(m_device);
            m_parser->parse();
        }
    });

    // execute and wait the end of the program
    return m_process->execute(execPath, args);
}

/**
 * QFileProxyRemove Implementation
 */
QFileProxyRemove::QFileProxyRemove(const QString& programPath, const QStringList& args, QFile *toRemove, QObject *parent)
    : QObject(parent)
    , m_file(toRemove)
    , m_process(new QProcess(this))
    , m_execPath(programPath)
{
    connect(m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, [this](int, QProcess::ExitStatus) {
        m_file->remove();
        delete m_file;
        m_file = nullptr;
    });

    connect(m_process, static_cast<void(QProcess::*)(QProcess::ProcessError)>(&QProcess::error),
            this, [this](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart)
            KMessageBox::error(
                qApp->activeWindow(),
                i18n("Unable to launch the process %1 (%2)", m_execPath, error),
                i18n("Valgrind Error"));
    });

    m_process->start(programPath, args);
}

QFileProxyRemove::~QFileProxyRemove()
{
    if (m_file)
        m_file->remove();

    delete m_process;
}

}
