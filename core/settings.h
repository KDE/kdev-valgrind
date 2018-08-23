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

#include <KConfigGroup>

namespace Valgrind
{

class SettingsBase
{
public:
    virtual ~SettingsBase() = default;

protected:
    explicit SettingsBase(const QString& configKeyPrefix);

    class ValueBase;

    template<typename T>
    class Value;

    using IntValue = Value<int>;
    using BoolValue = Value<bool>;
    using StringValue = Value<QString>;

    QString m_configKeyPrefix;
    QList<ValueBase*> m_values;
};

class SettingsBase::ValueBase
{
    friend class Settings;

public:
    virtual ~ValueBase() = default;

protected:
    virtual void load(const KConfigGroup& config, const QString& configKeyPrefix) = 0;
    virtual void save(KConfigGroup& config, const QString& configKeyPrefix) = 0;
    virtual QString cmdArg() = 0;
};

template<typename T>
class SettingsBase::Value : public SettingsBase::ValueBase
{
    friend class Settings;

public:
    Value(SettingsBase* settings, const QString& configKey, const QString& cmdKey, const T& defaultValue);
    ~Value() override = default;

    operator const T& () const;
    Value<T>& operator=(const T& value);

protected:
    void load(const KConfigGroup& config, const QString& configKeyPrefix) override;
    void save(KConfigGroup& config, const QString& configKeyPrefix) override;

    QString cmdArg() override;

private:
    QString m_configKey;
    QString m_cmdKey;

    T m_value;
    T m_defaultValue;
};

class Settings : public SettingsBase
{
public:
    StringValue extraParameters;

    void load(const KConfigGroup& config);
    void save(KConfigGroup& config);

    QStringList cmdArgs();

protected:
    explicit Settings(const QString& configKeyPrefix);
    ~Settings() override = default;
};

}
