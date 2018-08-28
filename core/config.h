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

#include <KConfigSkeleton>

namespace Valgrind
{

class Config : public KConfigSkeleton
{
    Q_OBJECT

public:
    ~Config() override;

    void setConfigGroup(const KConfigGroup& group);

    ItemInt* addCmdItemInt(const QString& name, int& reference, int defaultValue, const QString& cmdName);
    ItemBool* addCmdItemBool(const QString& name, bool& reference, bool defaultValue, const QString& cmdName);
    ItemString* addCmdItemString(const QString& name, QString& reference, const QString& defaultValue, const QString& cmdName);

    QStringList cmdArgs();

protected:
    explicit Config(const QString& group);

private:
    QString m_group;
    QString m_extraArgs;

    class CmdItem;
    QList<CmdItem*> m_cmdItems;
};

}
