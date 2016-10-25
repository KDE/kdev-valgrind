/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2002 Harald Fernengel <harry@kdevelop.org>
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
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

#pragma once

#include <outputview/outputexecutejob.h>

#include <QProcess>
#include <QUrl>

class KConfigGroup;
class QFile;

namespace KDevelop
{

class ILaunchConfiguration;

}

namespace valgrind
{

class Model;
class Plugin;
class Parser;

class Job : public KDevelop::OutputExecuteJob
{
    Q_OBJECT

public:
    Job(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent = nullptr);
    virtual ~Job();

    void start() override;
    using KDevelop::OutputExecuteJob::doKill;

    // Factory
    static Job* createToolJob(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent = nullptr);

private slots:
    void postProcessStdout(const QStringList& lines) override;
    void postProcessStderr(const QStringList& lines) override;

    void childProcessExited(int exitCode, QProcess::ExitStatus exitStatus) override;
    void childProcessError(QProcess::ProcessError processError) override;

protected:
    using t_valgrind_cfg_argarray = QString[][3];

    virtual void beforeStart(); // called before launching the process
    virtual void processStarted(); // called after the process has been launched
    virtual void processEnded(); // called when the process ended

    virtual void addToolArgs(QStringList& args, KConfigGroup& cfg) const = 0;

    void processModeArgs(QStringList& out,
                         const t_valgrind_cfg_argarray mode_args,
                         int mode_args_count,
                         KConfigGroup& cfg) const;

    QStringList buildCommandLine() const;

    QUrl m_workingDir;

    Model* m_model;
    Parser* m_parser;

    KDevelop::ILaunchConfiguration* m_launchcfg;
    Plugin* m_plugin;

    // The valgrind output file
    QFile* m_file;

    QStringList m_standardOutput;
    QStringList m_errorOutput;
    QStringList m_xmlOutput;
};

/**
 * This class is used for tools : massif, callgrind, cachegrind.
 * It permits to remove the generated output file when the reading process (massif visuazer, kcachegrind) has been killed
 */
class QFileProxyRemove : public QObject
{
    Q_OBJECT

public:
    QFileProxyRemove(const QString& programPath, const QStringList& args, QFile* toRemove, QObject* parent = nullptr);
    virtual ~QFileProxyRemove();

private:
    QFile* m_file;
    QProcess* m_process;
    QString m_execPath;
};

/**
 * This class is used for tool : callgrind, cachegrind
 * It permits to call an process to do a post treatment on the valgrind output
 */
class KProcessOutputToParser : public QObject
{
Q_OBJECT
public:
    KProcessOutputToParser(Parser* parser);
    ~KProcessOutputToParser();

    int execute(const QString& execPath, const QStringList& args);

private:
    QProcess* m_process;
    QIODevice* m_device;
    Parser* m_parser;
};

}
