/* This file is part of KDevelop
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

#include "valgrindjob.h"

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

#include <execute/executepluginconstants.h>

#include "valgrindmodel.h"
#include "valgrindplugin.h"

ValgrindJob::ValgrindJob( const QString& tool, KDevelop::ILaunchConfiguration* cfg, QObject* parent )
    : KDevelop::OutputJob(parent)
    , m_process(new KProcess(this))
    , m_job(0)
    , m_server(0)
    , m_connection(0)
    , m_model(new ValgrindModel(this))
    , m_applicationOutput(new KDevelop::ProcessLineMaker(this))
    , m_launchcfg( cfg )
    , m_tool( tool )
{
    setCapabilities( KJob::Killable );
    m_process->setOutputChannelMode(KProcess::SeparateChannels);
    m_model->setDevice(m_process);

    connect(m_process, SIGNAL(readyReadStandardOutput()), SLOT(readyReadStandardOutput()));
    connect(m_process, SIGNAL(readyReadStandardError()), SLOT(readyReadStandardError()));
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)), SLOT(processErrored(QProcess::ProcessError)));
}

void ValgrindJob::start()
{
    KConfigGroup grp = m_launchcfg->config();
    KDevelop::EnvironmentGroupList l(KGlobal::config());
    
    QString executable = ExecutePlugin::executableFromConfig( grp );
    QString envgrp = grp.readEntry( ExecutePlugin::environmentGroupEntry, "" );
    
    if( executable.isEmpty() )
    {
        setError(-1);
        setErrorText( i18n("No executable specified") );
        kWarning() << "no executable set";
    } else
    {
        KShell::Errors err;
        if( KShell::splitArgs( executable, KShell::TildeExpand | KShell::AbortOnMeta, &err ).isEmpty() || err != KShell::NoError )
        {
            
            setError( -1 );
            if( err == KShell::BadQuoting ) 
            {
                setErrorText( i18n("There is a quoting error in the executable "
                "for the launch configuration '%1'. "
                "Aborting start.", m_launchcfg->name() ) );
            } else 
            {   
                setErrorText( i18n("A shell meta character was included in the "
                "executable for the launch configuration '%1', "
                "this is not supported currently. Aborting start.", m_launchcfg->name() ) );
            }
            kWarning() << "executable has meta characters";
        }
    }
    
    if( envgrp.isEmpty() )
    {
        kWarning() << i18n("No environment group specified, looks like a broken "
        "configuration, please check run configuration '%1'. "
        "Using default environment group.", m_launchcfg->name() );
        envgrp = l.defaultGroup();
    }
    
    KShell::Errors err;
    QStringList arguments = KShell::splitArgs( grp.readEntry( ExecutePlugin::argumentsEntry, "" ), KShell::TildeExpand | KShell::AbortOnMeta, &err );
    if( err != KShell::NoError )
    {
        
        setError( -1 );
        if( err == KShell::BadQuoting ) 
        {
            setErrorText( i18n("There is a quoting error in the arguments for "
            "the launch configuration '%1'. Aborting start.", m_launchcfg->name() ) );
        } else 
        {   
            setErrorText( i18n("A shell meta character was included in the "
            "arguments for the launch configuration '%1', "
            "this is not supported currently. Aborting start.", m_launchcfg->name() ) );
        }
        kDebug() << "arguments have meta characters";
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

    if (!m_server) {
        m_server = new QTcpServer(this);
        if (!m_server->listen()) {
            kWarning() << "Could not open TCP socket for communication with Valgrind: "
                       << m_server->errorString();
            delete m_server;
            m_server = 0;
        }
        if(m_server)
            connect(m_server, SIGNAL(newConnection()), SLOT(newValgrindConnection()));
    }

    m_process->setEnvironment( l.createEnvironment( envgrp, m_process->systemEnvironment()) );
    m_process->setWorkingDirectory( grp.readEntry(ExecutePlugin::workingDirEntry, KUrl( QFileInfo( executable ).absolutePath() ) ).toLocalFile() );
    m_process->setProperty( "executable", executable );

    QStringList valgrindArgs;
    valgrindArgs += KShell::splitArgs( grp.readEntry( "Valgrind Arguments", "" ) );
    
//TODO: Properly set following options for valgrind (no existing code for this)
/*
ui->numCallers->setValue( cfg.readEntry( "Framestack Depth", 12 ) );
ui->maxStackFrame->setValue( cfg.readEntry( "Maximum Framestack Size", 2000000 ) );
ui->limitErrors->setChecked( cfg.readEntry( "Limit Errors", false ) );
ui->leakSummary->setCurrentIndex( cfg.readEntry( "Leak Summary", 1 ) );
ui->leakResolution->setCurrentIndex( cfg.readEntry( "Leak Resolution Matching", 0 ) );
ui->showReachable->setChecked( cfg.readEntry("Show Reachable Blocks", false ) );
ui->freeBlockList->setValue( cfg.readEntry( "Free Block List Size", 5000000 ) );
ui->separateThreads->setChecked( cfg.readEntry( "Separate Thread Reporting", false ) );
ui->simulateCache->setChecked( cfg.readEntry( "Full Cache Simulation", false ) );
ui->simulateHWPref->setChecked( cfg.readEntry( "Simulate Hardware Prefetcher", false ) );
ui->happensBefore->setCurrentIndex( cfg.readEntry("Extra Synchronization Events", 0 ) );
*/

    valgrindArgs << QString( "--tool=%1" ).arg( grp.readEntry( m_tool ) );
    valgrindArgs << "--xml=yes";
    if( m_server ) {
        valgrindArgs << QString( "--log-socket=127.0.0.1:%1").arg( m_server->serverPort() );
    }
    valgrindArgs << executable;
    valgrindArgs += arguments;
    m_process->setProgram( grp.readEntry( "Valgrind Executable", KUrl( "/usr/bin/valgrind" ) ).toLocalFile() );
    m_process->start();
}

bool ValgrindJob::doKill()
{
    m_process->kill();
    return true;
}

void ValgrindJob::readFromValgrind( )
{}

void ValgrindJob::newValgrindConnection()
{
    Q_ASSERT(m_server);


    QTcpSocket* sock = m_server->nextPendingConnection();
    if (!sock)
        return;

    if (m_connection) {
        kWarning() << "Got a new valgrind connection while old one was still alive!";
        delete sock; // discard new connection
    } else {
        m_connection = sock;
        m_model->setDevice(m_connection);
        connect(m_connection, SIGNAL(readyRead()), m_model, SLOT(parse()));
        connect(m_connection, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
    }
}

void ValgrindJob::socketError(QAbstractSocket::SocketError)
{
    Q_ASSERT(m_connection);
    
    //FIXME: The user should be notified about that but we cannot use KMessageBox because we might not be on
    //the UI thread.
    kWarning() << i18n("Socket error while communicating with valgrind: \"%1\"", m_connection->errorString()) <<
                          i18n("Valgrind communication error");
}

ValgrindPlugin * ValgrindJob::plugin() const
{
    return static_cast<ValgrindPlugin*>(const_cast<QObject*>(parent()));
}

void ValgrindJob::processErrored(QProcess::ProcessError e)
{
    switch (e) {
        case QProcess::FailedToStart:
            KMessageBox::error(qApp->activeWindow(), i18n("Failed to start valgrind from \"%1.\"", m_process->property("executable").toString()), i18n("Failed to start Valgrind"));
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

void ValgrindJob::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    kDebug() << "Process Finished, exitCode" << exitCode << "process exit status" << exitStatus;

    //core()->running( this, false );

    delete m_connection;
    m_connection = 0;

    delete m_server;
    m_server = 0;

    /*if (kcInfo.runKc)
    {
        KProcess kcProc;
//        kcProc.setWorkingDirectory(kcInfo.kcWorkDir);
        kcProc << kcInfo.kcPath;
        kcProc << QString("cachegrind.out.%1").arg(p->pid());
        kcProc.startDetached();
    }*/
}

void ValgrindJob::readyReadStandardError()
{
    if (m_connection) {
        m_applicationOutput->slotReceivedStderr(m_process->readAllStandardError());
        return;
    }

    m_model->parse();
}

void ValgrindJob::readyReadStandardOutput()
{
    m_applicationOutput->slotReceivedStdout(m_process->readAllStandardOutput());
}

KDevelop::OutputModel* ValgrindJob::model()
{
    return dynamic_cast<KDevelop::OutputModel*>( KDevelop::OutputJob::model() );
}

#include "valgrindjob.moc"