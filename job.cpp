/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
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
#include "memcheckjob.h"

#include "massifmodel.h"
#include "massifparser.h"
#include "massifjob.h"

#include "cachegrindmodel.h"
#include "cachegrindparser.h"
#include "cachegrindjob.h"

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
	else if (tool == "cachegrind")
	{
	    m_model = new valgrind::CachegrindModel();
	    m_parser = new valgrind::CachegrindParser();
	    QObject::connect(m_parser, SIGNAL(newItem(valgrind::ModelItem*)),
			     m_model, SLOT(newItem(valgrind::ModelItem*)));
	}

	QObject::connect(m_parser, SIGNAL(reset()), m_model, SLOT(reset()));
    }


    // The factory for jobs

    /*static */ Job	*Job::createToolJob( KDevelop::ILaunchConfiguration* cfg, valgrind::Plugin *inst, QObject* parent )
    {
	const QString &name = cfg->config().readEntry( "Current Tool", "memcheck" );
	if (name == "memcheck")
	    return new MemcheckJob(cfg, inst, parent);
	else if (name == "massif")
	    return new MassifJob(cfg, inst, parent);
	else if (name == "cachegrind")
	    return new CachegrindJob(cfg, inst, parent);
	return NULL;
    }

    Job::Job( KDevelop::ILaunchConfiguration* cfg, valgrind::Plugin *inst, QObject* parent )
	: KDevelop::OutputJob(parent)
	, m_process(new KProcess(this))
	, m_model(0)
	, m_parser(0)
	, m_applicationOutput(new KDevelop::ProcessLineMaker(this))
	, m_launchcfg( cfg )
	, m_plugin( inst )
        , m_killed( false )
    {
	setCapabilities( KJob::Killable );
	m_process->setOutputChannelMode( KProcess::SeparateChannels );
	connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)),
		SLOT(processFinished(int, QProcess::ExitStatus)));
	connect(m_process, SIGNAL(error(QProcess::ProcessError)),
		SLOT(processErrored(QProcess::ProcessError)));


	// create the correct model for each tool
	QString tool = m_launchcfg->config().readEntry( "Current Tool", "memcheck" );
	ModelParserFactoryPrivate factory;

        setTitle( QString( "valgrind (" + tool + ")" ));
	factory.make(tool, m_model, m_parser);
	m_model->job(this);
	// every job seems to overwrite this parameter, is it really useful ?
	//m_parser->setDevice(m_process);


#ifndef _UNIT_TESTS_
	m_plugin->incomingModel(m_model);
#endif

    }

    Job::~Job()
    {
	// these are allocated in the constructor
	delete m_process;
	delete m_applicationOutput;
	delete m_parser;
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
	QStringList		args;
	QString			tool = cfg.readEntry( "Current Tool", "memcheck" );

	args += KShell::splitArgs( cfg.readEntry( "Valgrind Arguments", "" ) );
	processModeArgs(args, generic_args, generic_args_count, cfg);

	addToolArgs(args, cfg);

	return args;
    }

    void Job::start()
    {
	KConfigGroup grp = m_launchcfg->config();
	IExecutePlugin* iface = KDevelop::ICore::self()->pluginController()->pluginForExtension("org.kdevelop.IExecutePlugin")->extension<IExecutePlugin>();
	KDevelop::EnvironmentGroupList l(KGlobal::config());
	QString envgrp = iface->environmentGroup( m_launchcfg );
	QString err;
	QString executable = iface->executable( m_launchcfg, err ).toLocalFile();

	Q_ASSERT(iface);
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

	connect(m_applicationOutput, SIGNAL(receivedStdoutLines(QStringList)),
		model(), SLOT(appendLines(QStringList)));
	connect(m_applicationOutput, SIGNAL(receivedStderrLines(QStringList)),
		model(), SLOT(appendLines(QStringList)));
	m_process->setEnvironment( l.createEnvironment( envgrp, m_process->systemEnvironment()) );

	Q_ASSERT(m_process->state() != QProcess::Running);

	KUrl wc = iface->workingDirectory( m_launchcfg );
	if( wc.isEmpty() || ! wc.isValid() )
	  wc = KUrl( QFileInfo( executable ).absolutePath() );
	m_process->setWorkingDirectory( wc.toLocalFile() );
	m_process->setProperty( "executable", executable );

	// some tools need to initialize stuff before the process starts
	this->beforeStart();

	QStringList valgrindArgs;

	valgrindArgs = buildCommandLine();
	valgrindArgs << executable;
	valgrindArgs += arguments;
	kDebug() << "executing:" << grp.readEntry( "Valgrind Executable", KUrl( "/usr/bin/valgrind" ) ).toLocalFile() << valgrindArgs;
	m_process->setProgram( grp.readEntry( "Valgrind Executable", KUrl( "/usr/bin/valgrind" ) ).toLocalFile(),
			       valgrindArgs );
	m_process->start();
	QString	s = i18n( "job running (pid=%1)", m_process->pid() );
	emit updateTabText(m_model, s);

	this->processStarted();
    }

    bool Job::doKill()
    {
        if ( m_process && m_process->pid() ) {
            m_process->kill();
            m_killed = true;
        }
	return true;
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
            // if the process was killed by the user, the crash was expected
            // don't notify the user
            if ( !m_killed )
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

	this->processEnded();

	emit updateTabText(m_model, tabname);
	emitResult();
    }

    void	Job::beforeStart()
    {
    }

    void	Job::processStarted()
    {
    }

    void	Job::processEnded()
    {
    }

    KDevelop::OutputModel* Job::model()
    {
	return dynamic_cast<KDevelop::OutputModel*>( KDevelop::OutputJob::model() );
    }

}


#include "job.moc"
