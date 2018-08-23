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

#include <kconfiggroup.h>

namespace Valgrind
{

class ISettingsBase
{
public:
    virtual ~ISettingsBase();

protected:
    explicit ISettingsBase(const QString& configKeyPrefix);

    class IValue;

    template<typename T>
    class Value;

    using IntValue = Value<int>;
    using BoolValue = Value<bool>;
    using StringValue = Value<QString>;

    QString m_configKeyPrefix;
    QList<IValue*> m_values;
};

class ISettingsBase::IValue
{
    friend class ISettings;

public:
    virtual ~IValue();

protected:
    virtual void load(const KConfigGroup& config, const QString& configKeyPrefix) = 0;
    virtual void save(KConfigGroup& config, const QString& configKeyPrefix) = 0;
    virtual QString cmdArg() = 0;
};

template<typename T>
class ISettingsBase::Value : public ISettingsBase::IValue
{
    friend class ISettings;

public:
    Value(ISettingsBase* settings, const QString& configKey, const QString& cmdKey, const T& defaultValue);
    ~Value() override;

    operator const T& () const;
    Value<T>& operator=(const T& value);

protected:
    void load(const KConfigGroup& config, const QString& configKeyPrefix) override;
    void save(KConfigGroup& config, const QString& configKeyPrefix) override;

    QString cmdArg() override;

    QString m_configKey;
    QString m_cmdKey;

    T m_value;
    T m_defaultValue;
};

class ISettings : public ISettingsBase
{
public:
    StringValue extraParameters;

    void load(const KConfigGroup& config);
    void save(KConfigGroup& config);

    QStringList cmdArgs();

protected:
    explicit ISettings(const QString& configKeyPrefix);
    ~ISettings() override;
};

}
