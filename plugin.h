/* This file is part of KDevelop
   Copyright 2002 Harald Fernengel <harry@kdevelop.org>
   Copyright 2007 Hamish Rodda <rodda@kde.org>
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

#include <interfaces/iplugin.h>

namespace KDevelop
{

class LaunchConfigurationType;
class ProblemModel;

}

namespace Valgrind
{

namespace Generic
{

class Job;

}

class LaunchMode;
class Launcher;
class WidgetFactory;

class Plugin : public KDevelop::IPlugin
{
    Q_OBJECT

public:
    Plugin(QObject* parent, const QVariantList& = QVariantList());
    ~Plugin() override;

    int configPages() const override;
    KDevelop::ConfigPage* configPage(int number, QWidget* parent) override;

    KDevelop::ProblemModel* problemModel() const;

    void jobReadyToStart(Generic::Job* job);
    void jobFinished(Generic::Job* job, bool ok);

signals:
    void addView(QWidget* view, const QString& name);

private:
    void runValgrind();

    WidgetFactory* m_factory;

    LaunchMode* m_launchMode;
    Launcher* m_launcher;
    KDevelop::LaunchConfigurationType* m_launchConfigurationType;

    KDevelop::ProblemModel* m_problemModel;

    QAction* m_runAction;
};

}
