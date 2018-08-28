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

#include "config.h"

#include "debug.h"

#include <qtcompat_p.h>

#include <KShell>

namespace Valgrind
{

class Config::CmdItem
{
public:
    CmdItem(ItemInt* item, const QString& cmdName)
        : CmdItem(static_cast<KConfigSkeletonItem*>(item), cmdName)
    {
        m_type = Int;
    }

    CmdItem(ItemBool* item, const QString& cmdName)
        : CmdItem(static_cast<KConfigSkeletonItem*>(item), cmdName)
    {
        m_type = Bool;
    }

    CmdItem(ItemString* item, const QString& cmdName)
        : CmdItem(static_cast<KConfigSkeletonItem*>(item), cmdName)
    {
        m_type = String;
    }

    ~CmdItem() = default;

    QString cmdArg()
    {
        QString value;

        switch (m_type) {
            case Int:
                value = QString::number(m_item->property().toInt());
                break;

            case Bool:
                value = m_item->property().toBool() ? QStringLiteral("yes") : QStringLiteral("no");
                break;

            case String:
                value = m_item->property().toString();
                break;
        }

        return QStringLiteral("--%1=%2").arg(m_cmdName, value);
    }

private:
    CmdItem(KConfigSkeletonItem* item, const QString& cmdName)
        : m_item(item)
        , m_cmdName(cmdName)
    {
        Q_ASSERT(item);
        Q_ASSERT(!cmdName.isEmpty());
    }

    enum Type
    {
        Int,
        Bool,
        String
    };

    KConfigSkeletonItem* m_item;
    QString m_cmdName;
    Type m_type;
};

Config::Config(const QString& group)
    : m_group(group)
{
    setCurrentGroup(group);

    addItemString(QStringLiteral("extraArgs"), m_extraArgs);
}

Config::~Config()
{
    qDeleteAll(m_cmdItems);
}

inline void addParent(QString& fullName, const KConfigGroup& parent)
{
    fullName = parent.name() + QLatin1Char('\x1d') + fullName;
}

// TODO create patch for KCoreConfigSkeleton::setCurrentGroup(const KConfigGroup& group) overload ?
void Config::setConfigGroup(const KConfigGroup& group)
{
    setSharedConfig(KSharedConfig::openConfig(group.config()->name(), group.config()->openFlags()));

    QString fullName(m_group);
    addParent(fullName, group);

    KConfigGroup parentGroup = group.parent();
    while (parentGroup.isValid() && parentGroup.name() != QStringLiteral("<default>")) {
        addParent(fullName, parentGroup);
        parentGroup = parentGroup.parent();
    }

    setCurrentGroup(fullName);

    const auto allItems = items();
    for (auto item : allItems) {
        item->setGroup(fullName);
    }
}

QStringList Config::cmdArgs()
{
    QStringList args;
    for (auto cmdItem : qAsConst(m_cmdItems)) {
        args += cmdItem->cmdArg();
    }
    args += KShell::splitArgs(m_extraArgs);
    args.removeAll(QStringLiteral(""));

    return args;
}

Config::ItemInt* Config::addCmdItemInt(const QString& name, int& reference, int defaultValue, const QString& cmdName)
{
    auto item = addItemInt(name, reference, defaultValue);
    m_cmdItems += new CmdItem(item, cmdName);
    return item;
}

Config::ItemBool* Config::addCmdItemBool(const QString& name, bool& reference, bool defaultValue, const QString& cmdName)
{
    auto item = addItemBool(name, reference, defaultValue);
    m_cmdItems += new CmdItem(item, cmdName);
    return item;
}

Config::ItemString* Config::addCmdItemString(const QString& name, QString& reference, const QString& defaultValue, const QString& cmdName)
{
    auto item = addItemString(name, reference, defaultValue);
    m_cmdItems += new CmdItem(item, cmdName);
    return item;
}

}
