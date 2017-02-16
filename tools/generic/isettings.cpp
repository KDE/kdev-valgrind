/* This file is part of KDevelop
   Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#include "isettings.h"

#include <kshell.h>

namespace Valgrind
{

ISettingsBase::ISettingsBase(const QString& configKeyPrefix)
    : m_configKeyPrefix(configKeyPrefix)
{
}

ISettingsBase::~ISettingsBase()
{
}

ISettingsBase::IValue::~IValue()
{
}

template<typename T>
ISettingsBase::Value<T>::Value(
    ISettingsBase* settings,
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
ISettingsBase::Value<T>::~Value()
{
}

template<typename T>
ISettingsBase::Value<T>::operator const T& () const
{
    return m_value;
}

template<typename T>
ISettingsBase::Value<T>& ISettingsBase::Value<T>::operator=(const T& value)
{
    m_value = value;
    return *this;
}

template<typename T>
void ISettingsBase::Value<T>::load(const KConfigGroup& config, const QString& configKeyPrefix)
{
    m_value = config.readEntry(QStringLiteral("%1 %2").arg(configKeyPrefix).arg(m_configKey),
                               m_defaultValue);
}

template<typename T>
void ISettingsBase::Value<T>::save(KConfigGroup& config, const QString& configKeyPrefix)
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
QString ISettingsBase::Value<T>::cmdArg()
{
    if (m_cmdKey.isEmpty()) {
        return QStringLiteral("");
    } else {
        return QString("--%1=%2").arg(m_cmdKey).arg(toCmdArg(m_value));
    }
}

template class ISettingsBase::Value<int>;
template class ISettingsBase::Value<bool>;
template class ISettingsBase::Value<QString>;

ISettings::ISettings(const QString& configKeyPrefix)
    : ISettingsBase(configKeyPrefix)

    , extraParameters(
        this,
        QStringLiteral("Extra Parameters"),
        QStringLiteral(""),
        QStringLiteral(""))
{
}

ISettings::~ISettings()
{
}

void ISettings::load(const KConfigGroup& config)
{
    for (auto value : m_values) {
        value->load(config, m_configKeyPrefix);
    }
}

void ISettings::save(KConfigGroup& config)
{
    for (auto value : m_values) {
        value->save(config, m_configKeyPrefix);
    }
}

QStringList ISettings::cmdArgs()
{
    QStringList args;
    for (auto value : m_values) {
        args += value->cmdArg();
    }
    args += KShell::splitArgs(extraParameters);
    args.removeAll(QStringLiteral(""));

    return args;
}

}
