/* This file is part of KDevelop
   Copyright 2002 Harald Fernengel <harry@kdevelop.org>
   Copyright 2007 Hamish Rodda <rodda@kde.org>
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

#include <interfaces/iplugin.h>

#include <QMultiHash>

class KJob;

namespace KDevelop
{

class ProblemModel;
class ILauncher;

}

namespace Valgrind
{

class IJob;
class ITool;
class LaunchMode;
class ProblemModel;
class ToolViewFactory;

class Plugin : public KDevelop::IPlugin
{
    Q_OBJECT

public:
    Plugin(QObject* parent, const QVariantList& = QVariantList());
    ~Plugin() override;

    static Plugin* self();

    void unload() override;

    int configPages() const override;
    KDevelop::ConfigPage* configPage(int number, QWidget* parent) override;

    LaunchMode* launchMode() const;
    ProblemModel* problemModel() const;

    void executeDefaultLaunch(const QString& launcherId);

    void jobReadyToStart(IJob* job);
    void jobReadyToFinish(IJob* job, bool ok);
    void jobFinished(KJob* job);

    bool isRunning();

Q_SIGNALS:
    void addView(QWidget* view, const QString& name);

private:
    void setupExecutePlugin(KDevelop::IPlugin* plugin, bool load);

    ToolViewFactory* m_toolViewFactory;

    LaunchMode* m_launchMode;
    QMultiHash<KDevelop::IPlugin*, KDevelop::ILauncher*> m_launchers;

    ProblemModel* m_problemModel;
    bool m_isRunning;

    QList<ITool*> m_tools;

    static Plugin* m_self;
};

}
