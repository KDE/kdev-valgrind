/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>

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

#include <QTcpServer>
#include <QTcpSocket>
#include <KProcess>
#include <kdebug.h>

#include <execute/iexecuteplugin.h>
#include <interfaces/icore.h>
#include <interfaces/ilaunchconfiguration.h>
#include <interfaces/iplugincontroller.h>

#include "massifjob.h"

namespace valgrind
{
    MassifJob::MassifJob(KDevelop::ILaunchConfiguration* cfg,
			 valgrind::Plugin *inst,
			 QObject* parent)
	: Job(cfg, inst, parent)
	, m_file(0)
    {
    }

    MassifJob::~MassifJob()
    {
	if (m_file)
	    delete m_file;
    }

    void MassifJob::processStarted()
    {
	QString filename = QString("%1/massif.out.%2").arg(m_workingDir.toLocalFile()).arg(m_process->pid());

	m_file = new QFile(filename);
    }

    void MassifJob::processEnded()
    {
	if (m_file)
	{
	    m_file->open(QIODevice::ReadOnly);
	    m_parser->setDevice(m_file);
	    m_parser->parse();
	    m_file->close();
	    m_file->remove();
	}
    }

    void MassifJob::addToolArgs(QStringList &args, KConfigGroup &cfg) const
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

}
