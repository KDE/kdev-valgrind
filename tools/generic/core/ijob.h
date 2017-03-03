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

#include <interfaces/istatus.h>
#include <kconfiggroup.h>
#include <outputview/outputexecutejob.h>

class QWidget;

namespace KDevelop
{

class ILaunchConfiguration;

}

namespace Valgrind
{

class ITool;

class IJob : public KDevelop::OutputExecuteJob, public KDevelop::IStatus
{
    Q_OBJECT
    Q_INTERFACES(KDevelop::IStatus)

public:
    IJob(const ITool* tool, KDevelop::ILaunchConfiguration* launchConfig);

    ~IJob() override;

    void start() override;
    using KDevelop::OutputExecuteJob::doKill;

    const ITool* tool() const;
    virtual QWidget* createView() = 0;

    QString statusName() const override;

signals:
    void clearMessage(KDevelop::IStatus*) override;
    void hideProgress(KDevelop::IStatus*) override;
    void showErrorMessage(const QString& message, int timeout = 0) override;
    void showMessage(KDevelop::IStatus*, const QString& message, int timeout = 0) override;
    void showProgress(KDevelop::IStatus*, int minimum, int maximum, int value) override;

protected:
    void postProcessStderr(const QStringList& lines) override;
    virtual void processValgrindOutput(const QStringList& lines);

    void childProcessExited(int exitCode, QProcess::ExitStatus exitStatus) override;
    void childProcessError(QProcess::ProcessError processError) override;

    virtual bool processEnded();

    virtual void addLoggingArgs(QStringList& args) const;
    virtual void addToolArgs(QStringList& args) const = 0;

    int executeProcess(const QString& executable, const QStringList& args, QByteArray& processOutput);

    QStringList buildCommandLine() const;

    const ITool* m_tool;

    KConfigGroup m_config;

    QString m_analyzedExecutable;
    QStringList m_analyzedExecutableArguments;

    QStringList m_valgrindOutput;

    quint16 m_tcpServerPort;
};

}
