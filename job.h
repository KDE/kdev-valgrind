/* This file is part of KDevelop
 * Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
 * Copyright 2002 Harald Fernengel <harry@kdevelop.org>
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>

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

#ifndef VALGRINDJOB_H
#define VALGRINDJOB_H

#include <QProcess>
#include <QTcpSocket>

#include <outputview/outputjob.h>

#include "parser.h"

class KJob;
class KProcess;
class QXmlInputSource;
class QXmlSimpleReader;
class QTcpServer;
class QTcpSocket;
class QBuffer;

namespace KDevelop
{
class ProcessLineMaker;
class ILaunchConfiguration;
class OutputModel;
class ILaunchConfiguration;
}
namespace valgrind
{
  class Plugin;
  class Parser;

  class Job : public KDevelop::OutputJob
  {
    Q_OBJECT

  public:
      Job(KDevelop::ILaunchConfiguration* cfg, valgrind::Plugin *inst, QObject* parent = 0);
      virtual ~Job();

      valgrind::Plugin* plugin() const;

      virtual void start();
  protected:
      virtual bool doKill();

  private slots:
      void newValgrindConnection();
      void socketError(QAbstractSocket::SocketError err);
      void readFromValgrind();

      void readyReadStandardOutput();
      void readyReadStandardError();
      void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
      void processErrored(QProcess::ProcessError);

  private:
      typedef QString	t_valgrind_cfg_argarray[][3];

      void	processModeArgs(QStringList & out,
          const t_valgrind_cfg_argarray mode_args,
          int mode_args_count,
          KConfigGroup & cfg) const;

      void	addMemcheckArgs(QStringList &args, KConfigGroup &cfg) const;
      void	addMassifArgs(QStringList &args, KConfigGroup &cfg) const;
      QStringList	buildCommandLine() const;

      KDevelop::OutputModel* model();


      KProcess* m_process;
      int m_currentPid;
      KJob* m_job;

      QTcpServer* m_server;
      QTcpSocket* m_connection;

      valgrind::Model* m_model;
      valgrind::Parser m_parser;

      KDevelop::ProcessLineMaker* m_applicationOutput;
      KDevelop::ILaunchConfiguration* m_launchcfg;
      valgrind::Plugin *m_plugin;
  };

}
#endif
