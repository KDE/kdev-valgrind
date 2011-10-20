/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2002 Harald Fernengel <harry@kdevelop.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "job.h"

#include <QXmlInputSource>
#include <QXmlSimpleReader>
#include <QTcpServer>
#include <QTcpSocket>
#include <QApplication>
#include <QBuffer>
#include <QFileInfo>

#include <klocale.h>
#include <kdebug.h>
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

#include "memcheckmodel.h"
#include "memcheckparser.h"
#include "massifmodel.h"
#include "massifparser.h"
#include "plugin.h"

namespace valgrind
{
Job::Job( KDevelop::ILaunchConfiguration* cfg, valgrind::Plugin *inst, QObject* parent )
    : KDevelop::OutputJob(parent)
    , m_process(new KProcess(this))
    , m_job(0)
    , m_server(0)
    , m_connection(0)
    , m_model(0)
    , m_parser(0)
    , m_applicationOutput(new KDevelop::ProcessLineMaker(this))
    , m_launchcfg( cfg )
    , m_plugin(inst)
{
    QString tool = m_launchcfg->config().readEntry( "Current Tool", "memcheck" );
    // create the correct model for each tool
    if (tool == "memcheck")
      {
	m_model = new valgrind::MemcheckModel();
	m_parser = new valgrind::MemcheckParser();
      }
    else if (tool == "massif")
      {
	m_model = new valgrind::MassifModel();
	m_parser = new valgrind::MassifParser();
      }

    setCapabilities( KJob::Killable );
    m_process->setOutputChannelMode(KProcess::SeparateChannels);
    m_parser->setDevice(m_process);

    connect(m_process, SIGNAL(readyReadStandardOutput()), SLOT(readyReadStandardOutput()));
    connect(m_process, SIGNAL(readyReadStandardError()), SLOT(readyReadStandardError()));
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)), SLOT(processErrored(QProcess::ProcessError)));
    // connect the parser and the model
    connect(m_parser, SIGNAL(newElement(valgrind::Model::eElementType)),
            m_model, SLOT(newElement(valgrind::Model::eElementType)));
    connect(m_parser, SIGNAL(newData(valgrind::Model::eElementType, QString, QString)),
            m_model, SLOT(newData(valgrind::Model::eElementType, QString, QString)));
    connect(m_parser, SIGNAL(reset()), m_model, SLOT(reset()));

#ifndef _UNIT_TESTS_
    m_plugin->incomingModel(m_model);
#endif
}

Job::~Job()
{
}

void		Job::processModeArgs(QStringList & out,
                                 const t_valgrind_cfg_argarray mode_args,
                                 int mode_args_count,
                                 KConfigGroup & cfg) const
{
    // For each option, set the right string in the arguments list
    for (int i = 0; i < mode_args_count; ++i)
    {
        QString val;
        QString argtype = mode_args[i][2];

        if (argtype == "str")
            val = cfg.readEntry( mode_args[i][0] );
        else if (argtype == "int")
        {
            int n = cfg.readEntry( mode_args[i][0], 0 );
            if (n)
                val.sprintf("%d", n);
        }
        else if (argtype == "bool")
        {
            bool n = cfg.readEntry( mode_args[i][0], false );
            val = n ? "yes" : "no";
        }
        if (val.length()) // Finally, if our argument is meaningful, add it
        {
            QString argument = QString(mode_args[i][1]) + val;
            out << argument;
        }
    }
}

void		Job::addMemcheckArgs(QStringList &args, KConfigGroup &cfg) const
{
    static const t_valgrind_cfg_argarray memcheck_args =
    {
        {"Memcheck Arguments",	"",			"str"},
        {"Freelist Size",		"--freelist-vol=",	"int"},
        {"Show Reachable",		"--show-reachable=",	"bool"},
	//        {"Track Origins",		"--track-origins=",	"bool"},
        {"Undef Value Errors",	"--undef-value-errors=","bool"}
    };
    static const int		memcheck_args_count = sizeof(memcheck_args) / sizeof(*memcheck_args);
    args << "--xml=yes";
    if( m_server )
    {
        args << QString( "--xml-socket=127.0.0.1:%1").arg( m_server->serverPort() );
    }
    processModeArgs(args, memcheck_args, memcheck_args_count, cfg);
}


void		Job::addMassifArgs(QStringList &args, KConfigGroup &cfg) const
{
    Q_UNUSED(args);
    Q_UNUSED(cfg);
}

QStringList	Job::buildCommandLine() const
{
    static const t_valgrind_cfg_argarray generic_args =
    {
        {"Current Tool",		"--tool=",		"str"},
        {"Stackframe Depth",	"--num-callers=",	"int"}, // TODO: Rather use defines/enums for the
        {"Maximum Stackframe Size", "--max-stackframe=",	"int"}, // types. There probably are some in KDevelop
        {"Limit Errors",		"--error-limit=",	"bool"} // already.
    };
    static const int		generic_args_count = sizeof(generic_args) / sizeof(*generic_args);

    KConfigGroup		cfg = m_launchcfg->config();
    QStringList			args;
    QString			    tool = cfg.readEntry( "Current Tool", "memcheck" );

    args += KShell::splitArgs( cfg.readEntry( "Valgrind Arguments", "" ) );
    processModeArgs(args, generic_args, generic_args_count, cfg);

    if (tool == "memcheck")
        addMemcheckArgs(args, cfg);
    else if (tool == "massif")
        addMassifArgs(args, cfg);

    return args;
}

void Job::start()
{
    KConfigGroup grp = m_launchcfg->config();

    IExecutePlugin* iface = KDevelop::ICore::self()->pluginController()->pluginForExtension("org.kdevelop.IExecutePlugin")->extension<IExecutePlugin>();
    Q_ASSERT(iface);

    KDevelop::EnvironmentGroupList l(KGlobal::config());
    QString envgrp = iface->environmentGroup( m_launchcfg );

    QString err;
    QString executable = iface->executable( m_launchcfg, err ).toLocalFile();

    if( !err.isEmpty() )
    {
        setError( -1 );
        setErrorText( err );
        return;
    }

    if( envgrp.isEmpty() )
    {
        kWarning() << i18n("No environment group specified, looks like a broken "
                           "configuration, please check run configuration '%1'. "
                           "Using default environment group.", m_launchcfg->name() );
        envgrp = l.defaultGroup();
    }

    QStringList arguments = iface->arguments( m_launchcfg, err );
    if( !err.isEmpty() )
    {
        setError( -1 );
        setErrorText( err );
    }
    if( error() != 0 )
    {
        emitResult();
        return;
    }

    setStandardToolView(KDevelop::IOutputView::DebugView);
    setBehaviours(KDevelop::IOutputView::AllowUserClose | KDevelop::IOutputView::AutoScroll);
    setModel( new KDevelop::OutputModel(), KDevelop::IOutputView::TakeOwnership );

    startOutput();

    connect(m_applicationOutput, SIGNAL(receivedStdoutLines(QStringList)), model(), SLOT(appendLines(QStringList)));
    connect(m_applicationOutput, SIGNAL(receivedStderrLines(QStringList)), model(), SLOT(appendLines(QStringList)));

    Q_ASSERT(m_process->state() != QProcess::Running);

    if (!m_server)
    {
        m_server = new QTcpServer(this);
        if (!m_server->listen())
        {
            kWarning() << "Could not open TCP socket for communication with Valgrind: "
            << m_server->errorString();
            delete m_server;
            m_server = 0;
        }
        if(m_server)
            connect(m_server, SIGNAL(newConnection()), SLOT(newValgrindConnection()));
    }

    m_process->setEnvironment( l.createEnvironment( envgrp, m_process->systemEnvironment()) );
    KUrl wc = iface->workingDirectory( m_launchcfg );
    if( wc.isEmpty() || ! wc.isValid() )
    {
        wc = KUrl( QFileInfo( executable ).absolutePath() );
    }
    m_process->setWorkingDirectory( wc.toLocalFile() );
    m_process->setProperty( "executable", executable );

    QStringList valgrindArgs;

    valgrindArgs = buildCommandLine();
    valgrindArgs << executable;
    valgrindArgs += arguments;
    kDebug() << "executing:" << grp.readEntry( "Valgrind Executable", KUrl( "/usr/bin/valgrind" ) ).toLocalFile() << valgrindArgs;
    m_process->setProgram( grp.readEntry( "Valgrind Executable", KUrl( "/usr/bin/valgrind" ) ).toLocalFile(),
                           valgrindArgs );

    /*
      /!\ Massif specific :
      this is a trick : valgrind in run in a new process because it seem that m_server need to receive
      the data before the end of m_process. Now m_process remove the file created by massif.
      We need to do something better than that ! But it works for now to develop the new parser and model
    */
    QString tool = m_launchcfg->config().readEntry( "Current Tool", "memcheck" );
    if (tool == "massif")
      {
	int pid;
	KProcess* process2 = new KProcess(this);
	process2->setOutputChannelMode(KProcess::SeparateChannels);
	process2->setWorkingDirectory( wc.toLocalFile() );
	process2->setProperty( "executable", executable );
	process2->setProgram(grp.readEntry( "Valgrind Executable", KUrl( "/usr/bin/valgrind" ) ).toLocalFile(),
                           valgrindArgs );
	process2->start();
	pid = process2->pid();
	process2->waitForFinished();
	QString massifFile = QString("/home/damien/projects/MassifTest/build/massif.out.%1").arg(pid);
	m_process->setProgram("rm", QStringList(massifFile));
	QTcpSocket *tcpSocket;
	tcpSocket = new QTcpSocket(this);
	tcpSocket->connectToHost("127.0.0.1", m_server->serverPort(), QIODevice::WriteOnly);
	if (tcpSocket->waitForConnected(1000))
	  {
	    QFile file(massifFile);
	    file.open(QIODevice::ReadOnly);
	    QByteArray ba = file.readAll();
	    file.close();
	    tcpSocket->write(ba);
	  }
	tcpSocket->flush(); // it seems that fush is not the best method to use to wait the end of the write !
	tcpSocket->close();
      }
    // End Massif stuff /!\

    m_process->start();
}

bool Job::doKill()
{
    m_process->kill();
    return true;
}

void Job::readFromValgrind( )
{}

void Job::newValgrindConnection()
{
    Q_ASSERT(m_server);

    QTcpSocket* sock = m_server->nextPendingConnection();
    kDebug() << "got valgrind connection:" << sock;
    if (!sock)
        return;

    if (m_connection)
    {
        kWarning() << "Got a new valgrind connection while old one was still alive!";
        delete sock; // discard new connection
    }
    else
    {
        // The connection is successfull we connect the parser to the ??
        m_connection = sock;
        //TODO still USEFUL?
        m_parser->setDevice(m_connection);
        // Connects the parser to the socket
        connect(m_connection, SIGNAL(readyRead()), m_parser, SLOT(parse()));
        connect(m_connection, SIGNAL(error(QAbstractSocket::SocketError)),
                SLOT(socketError(QAbstractSocket::SocketError)));
    }
}

void Job::socketError(QAbstractSocket::SocketError)
{
    Q_ASSERT(m_connection);

    //FIXME: The user should be notified about that but we cannot use KMessageBox because we might not be on
    //the UI thread.
    kWarning() << i18n("Socket error while communicating with valgrind: \"%1\"", m_connection->errorString()) <<
    i18n("Valgrind communication error");
}

valgrind::Plugin * Job::plugin() const
{
    return m_plugin;
}

void Job::processErrored(QProcess::ProcessError e)
{
    switch (e)
    {
    case QProcess::FailedToStart:
        KMessageBox::error(qApp->activeWindow(), i18n("Failed to start valgrind from \"%1\".", m_process->property("executable").toString()), i18n("Failed to start Valgrind"));
        break;
    case QProcess::Crashed:
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
    kDebug() << "Process Finished, exitCode" << exitCode << "process exit status" << exitStatus;

    emitResult();
}

void Job::readyReadStandardError()
{
    if (m_connection)
    {
        m_applicationOutput->slotReceivedStderr(m_process->readAllStandardError());
        return;
    }
    //    m_parser.parse();
}

void Job::readyReadStandardOutput()
{
    m_applicationOutput->slotReceivedStdout(m_process->readAllStandardOutput());
}

KDevelop::OutputModel* Job::model()
{
    return dynamic_cast<KDevelop::OutputModel*>( KDevelop::OutputJob::model() );
}

}


#include "job.moc"
