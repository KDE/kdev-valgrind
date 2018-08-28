/* This file is part of KDevelop
   Copyright 2018 Anton Anikin <anton@anikin.xyz>

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

#include <interfaces/launchconfigurationpage.h>

class KConfigDialogManager;

namespace Valgrind
{

class Config;

class ConfigPage : public KDevelop::LaunchConfigurationPage
{
    Q_OBJECT

public:
    ~ConfigPage() override;

    QString title() const override final;
    QIcon icon() const override final;

    void loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject* project = nullptr) override final;
    void saveToConfiguration(KConfigGroup cfg, KDevelop::IProject* project = nullptr) const override final;

protected:
    explicit ConfigPage(QString title, QWidget* parent = nullptr);

    void init(Config* config);

    virtual void check();

private:
    QString m_title;

    QScopedPointer<Config> m_config;
    QScopedPointer<KConfigDialogManager> m_manager;

};

}
