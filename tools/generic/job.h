/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2002 Harald Fernengel <harry@kdevelop.org>
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2016-2017 Anton Anikin <anton.anikin@htower.ru>

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

#include <kconfiggroup.h>
#include <outputview/outputexecutejob.h>

#include <QProcess>
#include <QUrl>

class QWidget;

namespace KDevelop
{

class ILaunchConfiguration;

}

namespace Valgrind
{

class Plugin;

namespace Generic
{

class Job : public KDevelop::OutputExecuteJob
{
    Q_OBJECT

public:
    Job(KDevelop::ILaunchConfiguration* cfg,
        QString tool,
        bool hasView,
        Plugin* plugin,
        QObject* parent);

    ~Job() override;

    void start() override;
    using KDevelop::OutputExecuteJob::doKill;

    QString tool() const;
    QString target() const;

    bool hasView();
    virtual QWidget* createView() = 0;

    // Factory
    static Job* create(KDevelop::ILaunchConfiguration* cfg, Plugin* plugin, QObject* parent = nullptr);

protected:
    void postProcessStderr(const QStringList& lines) override;

    void childProcessExited(int exitCode, QProcess::ExitStatus exitStatus) override;
    void childProcessError(QProcess::ProcessError processError) override;

    virtual bool processEnded();

    virtual void addToolArgs(QStringList& args) const = 0;

    QStringList argValue(const QString& line) const;
    QString argValue(bool value) const;
    QString argValue(int value) const;

    int executeProcess(const QString& executable, const QStringList& args, QByteArray& processOutput);

    QStringList buildCommandLine() const;

    KConfigGroup m_config;

    QString m_tool;
    bool m_hasView;

    Plugin* m_plugin;

    QString m_analyzedExecutable;
    QStringList m_analyzedExecutableArguments;

    QUrl m_workingDir;

    QStringList m_errorOutput;
};

}

}
