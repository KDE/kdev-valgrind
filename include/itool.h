/* This file is part of KDevelop
   Copyright 2017 Anton Anikin <anton@anikin.xyz>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING. If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#pragma once

#include <QString>

namespace KDevelop
{

class ILauncher;
class ILaunchConfiguration;
class LaunchConfigurationPageFactory;

}

namespace Valgrind
{

class IJob;

class ITool
{
public:
    virtual ~ITool();

    /// Short name, used in UI.
    /// Example: i18n("Memcheck").
    QString name() const;

    /// Full name, used in UI (description).
    /// Example: i18n("Memcheck: a memory error detector").
    QString fullName() const;

    /// Internal Valgrind tool name, used in jobs.
    /// Example: "memcheck".
    QString valgrindToolName() const;

    /// Internal id, used as id for launcher and setting names prefix.
    /// Example: "Memcheck".
    QString id() const;

    /// "Analyze" menu action name, used when tool action added to action collection.
    /// Example: "memcheck_tool".
    QString menuActionName() const;

    /// True if tool has view
    bool hasView() const;

    KDevelop::ILauncher* createLauncher() const;

    virtual IJob* createJob(KDevelop::ILaunchConfiguration* launchConfig) const = 0;

    virtual KDevelop::LaunchConfigurationPageFactory* createConfigPageFactory() const = 0;

protected:
    ITool(
        const QString& id,
        const QString& name,
        const QString& fullName,
        const QString& valgrindToolName,
        const QString& menuActionName,
        bool hasView
    );

    QString m_id;
    QString m_name;
    QString m_fullName;
    QString m_valgrindToolName;
    QString m_menuActionName;
    bool m_hasView;
};

}
