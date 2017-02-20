/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2009 Andreas Pakulat <apaku@gmx.de>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
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

#include <interfaces/ilauncher.h>

class KJob;

namespace Valgrind
{

class IJob;
class LaunchMode;
class Plugin;

class ILauncher : public KDevelop::ILauncher
{
public:
    ILauncher(Plugin* plugin,
              LaunchMode* mode,
              const QString& name,
              const QString& description,
              const QString& id,
              KDevelop::LaunchConfigurationPageFactory* configPageFactory);
    ~ILauncher() override;

    QString name() const override final;
    QString description() const override final;
    QString id() override final;

    QStringList supportedModes() const override final;

    QList<KDevelop::LaunchConfigurationPageFactory*> configPages() const override final;

    KJob* start(const QString& launchMode, KDevelop::ILaunchConfiguration* config) override final;

protected:
    virtual IJob* createJob(KDevelop::ILaunchConfiguration* config, QObject* parent) = 0;

    Plugin* m_plugin;
    LaunchMode* m_mode;

    QString m_name;
    QString m_description;
    QString m_id;

    QList<KDevelop::LaunchConfigurationPageFactory*> m_configPageFactories;
};

}
