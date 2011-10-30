/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
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
#include <assert.h>

#include "memcheckmodel.h"
#include "memcheckparser.h"
#include "massifmodel.h"
#include "massifparser.h"
#include "plugin.h"

namespace valgrind
{
    /*!
     * Creates a model and a parser according to the specified name and
     * connects the 2 items
     */
    class ModelParserFactoryPrivate
    {

    public:
	void make(const QString &type, valgrind::Model* &m_model, valgrind::Parser* &m_parser);

    };

    void ModelParserFactoryPrivate::make(const QString &tool, valgrind::Model* &m_model, valgrind::Parser* &m_parser)
    {
	if (tool == "memcheck")
	{
	    m_model = new valgrind::MemcheckModel();
	    m_parser = new valgrind::MemcheckParser();
	    QObject::connect(m_parser, SIGNAL(newElement(valgrind::Model::eElementType)),
			     m_model, SLOT(newElement(valgrind::Model::eElementType)));
	    QObject::connect(m_parser, SIGNAL(newData(valgrind::Model::eElementType, QString, QString)),
			     m_model, SLOT(newData(valgrind::Model::eElementType, QString, QString)));
	}
	else if (tool == "massif")
	{
	    m_model = new valgrind::MassifModel();
	    m_parser = new valgrind::MassifParser();
	    QObject::connect(m_parser, SIGNAL(newItem(valgrind::ModelItem*)),
			     m_model, SLOT(newItem(valgrind::ModelItem*)));

	}

	QObject::connect(m_parser, SIGNAL(reset()), m_model, SLOT(reset()));
    }


    Job::Job( KDevelop::ILaunchConfiguration* cfg, valgrind::Plugin *inst, QObject* parent )
	: KDevelop::OutputJob(parent)
	, m_process(new KProcess(this))
	, m_tabIndex(0)
	, m_server(0)
	, m_connection(0)
	, m_model(0)
	, m_parser(0)
	, m_applicationOutput(new KDevelop::ProcessLineMaker(this))
	, m_launchcfg( cfg )
	, m_plugin(inst)
	, m_file(0)
    {
	QString tool = m_launchcfg->config().readEntry( "Current Tool", "memcheck" );
	// create the correct model for each tool

	setCapabilities( KJob::Killable );
	m_process->setOutputChannelMode(KProcess::SeparateChannels);

	// connect the parser and the model
	ModelParserFactoryPrivate factory;
	factory.make(tool, m_model, m_parser);
	m_model->setJob(this);
	m_parser->setDevice(m_process);

	connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(processFinished(int, QProcess::ExitStatus)));
	connect(m_process, SIGNAL(error(QProcess::ProcessError)), SLOT(processErrored(QProcess::ProcessError)));

#ifndef _UNIT_TESTS_
	m_plugin->incomingModel(m_model);
#endif
    }

    Job::~Job()
    {
	// We don't need this connection anymore
	if (m_connection)
	{
	    m_connection->close();
	    delete m_connection;
	}
	// This server either (TODO: could the server be static ?)
	if (m_server)
	{
	    m_server->close();
	    delete m_server;
	}

	if (m_file) // a file was used by valgrind (e.g. with massif or cachegrind)
	{
	    m_file->close();
	    m_file->remove();
	    delete m_file;
	}

	// these are allocated in the constructor
	delete m_process;
	delete m_applicationOutput;
	delete m_parser;

	//We cannot delete the model here, or it won't show up in the GUI: is it done somewhere else?

	//delete m_model;
    }

    void		Job::setTabIndex(int index)
    {
	m_tabIndex = index;
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
	    else if (argtype == "float")
	    {
		// FIXME: fake float
		int n = cfg.readEntry( mode_args[i][0], 1 );

		val.sprintf("%d.0", n);

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
		{"Memcheck Arguments", "", "str"},
		{"Freelist Size", "--freelist-vol=", "int"},
		{"Show Reachable", "--show-reachable=", "bool"},
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
	static const t_valgrind_cfg_argarray massif_args =
	    {
		{"Massif Arguments", "", "str"},
		{"depth", "--depth=", "int"},
		{"threshold", "--threshold=", "float"},
		{"peakInaccuracy", "--peak-inaccuracy=", "float"},
		{"maxSnapshots", "--max-snapshots=", "int"},
		{"snapshotFreq", "--detailed-freq=", "int"},
		{"profileHeap", "--heap=", "bool"},
		{"profileStack", "--stacks=", "bool"}
	    };
	static const int count = sizeof(massif_args) / sizeof(*massif_args);
    
	processModeArgs(args, massif_args, count, cfg);

	int tu = cfg.readEntry("timeUnit", 0);    
	if (tu == 0)
	    args << QString("--time-unit=i");
	else if (tu == 1)
	    args << QString("--time-unit=ms");
	else if (tu == 2)
	    args << QString("--time-unit=B");
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


	m_process->start();
	QString	s = i18n( "job running (pid=%1)", m_process->pid() );
	emit updateTabText(m_tabIndex, s);


	/* Some Valgrind tools do not write to sockets, so we will
	** have to read the output file when the process ends.
	*/
	QString tool = m_launchcfg->config().readEntry( "Current Tool", "memcheck" );
	QString filename;

	if (tool == "massif")
	    filename = QString("%1/massif.out.%2").arg(wc.toLocalFile()).arg(m_process->pid());
	else if (tool == "cachegrind")
	    filename = QString("%1/massif.out.%2").arg(wc.toLocalFile()).arg(m_process->pid());

	if (filename.length())
	    m_file = new QFile(filename);

    }

    bool Job::doKill()
    {
	m_process->kill();
	QString	s = i18n( "job killed" );
	emit updateTabText(m_tabIndex, s);
	return true;
    }

    void Job::newValgrindConnection()
    {
	Q_ASSERT(m_server);

	QTcpSocket* sock = m_server->nextPendingConnection();
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
	    //TODO still USEFUL? Probably, since parsers read from their device() attribute
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

	// Update: Sort of fixed, the stderr of valgrind is checked at the end of the process. The only
	// reason for a socket error to happen here is because Valgrind could not be launched, which is
	// precisely what is handled in processFinished
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
	qDebug() << "Process Finished, exitCode" << exitCode << "process exit status" << exitStatus;

	QString	tabname = i18n( "job finished (%1)", exitCode );

	if (exitCode != 0)
	{
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

	    if (s.startsWith("valgrind:"))
	    {
		QString err = s.split("\n")[0];
		err = err.replace("valgrind:", "");
		err += "\n\nPlease review your Valgrind launch configuration.";

		KMessageBox::error(qApp->activeWindow(), err, i18n("Valgrind Error"));
		tabname = i18n( "job failed");
	    }
	}

	if (m_file) //valgrind wrote some infos in m_file, parse it
	{
	    m_file->open(QIODevice::ReadOnly);
	    m_parser->setDevice(m_file);
	    m_parser->parse();
	}

	emit updateTabText(m_tabIndex, tabname);
	emitResult();
    }

    KDevelop::OutputModel* Job::model()
    {
	return dynamic_cast<KDevelop::OutputModel*>( KDevelop::OutputJob::model() );
    }

}


#include "job.moc"
