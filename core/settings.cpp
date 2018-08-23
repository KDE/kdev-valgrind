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

#include "settings.h"

#include <qtcompat_p.h>

#include <KShell>

namespace Valgrind
{

SettingsBase::SettingsBase(const QString& configKeyPrefix)
    : m_configKeyPrefix(configKeyPrefix)
{
}

template<typename T>
SettingsBase::Value<T>::Value(
    SettingsBase* settings,
    const QString& configKey,
    const QString& cmdKey,
    const T& defaultValue)

    : m_configKey(configKey)
    , m_cmdKey(cmdKey)
    , m_value(defaultValue)
    , m_defaultValue(defaultValue)
{
    Q_ASSERT(settings);
    Q_ASSERT(!configKey.isEmpty());

    settings->m_values.append(this);
}

template<typename T>
SettingsBase::Value<T>::operator const T& () const
{
    return m_value;
}

template<typename T>
SettingsBase::Value<T>& SettingsBase::Value<T>::operator=(const T& value)
{
    m_value = value;
    return *this;
}

template<typename T>
void SettingsBase::Value<T>::load(const KConfigGroup& config, const QString& configKeyPrefix)
{
    m_value = config.readEntry(QStringLiteral("%1 %2").arg(configKeyPrefix).arg(m_configKey),
                               m_defaultValue);
}

template<typename T>
void SettingsBase::Value<T>::save(KConfigGroup& config, const QString& configKeyPrefix)
{
    config.writeEntry(QStringLiteral("%1 %2").arg(configKeyPrefix).arg(m_configKey),
                      m_value);
}

inline QString toCmdArg(int value)
{
    return QString::number(value);
}

inline QString toCmdArg(bool value)
{
    return value ? QStringLiteral("yes") : QStringLiteral("no");
}

inline QString toCmdArg(const QString& value)
{
    return value;
}

template<typename T>
QString SettingsBase::Value<T>::cmdArg()
{
    if (m_cmdKey.isEmpty()) {
        return QStringLiteral("");
    } else {
        return QStringLiteral("--%1=%2").arg(m_cmdKey, toCmdArg(m_value));
    }
}

template class SettingsBase::Value<int>;
template class SettingsBase::Value<bool>;
template class SettingsBase::Value<QString>;

Settings::Settings(const QString& configKeyPrefix)
    : SettingsBase(configKeyPrefix)

    , extraParameters(
        this,
        QStringLiteral("Extra Parameters"),
        QStringLiteral(""),
        QStringLiteral(""))
{
}

void Settings::load(const KConfigGroup& config)
{
    for (auto value : qAsConst(m_values)) {
        value->load(config, m_configKeyPrefix);
    }
}

void Settings::save(KConfigGroup& config)
{
    for (auto value : qAsConst(m_values)) {
        value->save(config, m_configKeyPrefix);
    }
}

QStringList Settings::cmdArgs()
{
    QStringList args;
    for (auto value : qAsConst(m_values)) {
        args += value->cmdArg();
    }
    args += KShell::splitArgs(extraParameters);
    args.removeAll(QStringLiteral(""));

    return args;
}

}
