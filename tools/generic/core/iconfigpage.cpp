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

#include "iconfigpage.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>

namespace Valgrind
{

IConfigPage::IConfigPage(QWidget* parent)
    : LaunchConfigurationPage(parent)
{
}

void IConfigPage::connectToChanged(QCheckBox* box)
{
    connect(box, &QCheckBox::toggled, this, &LaunchConfigurationPage::changed);
}

void IConfigPage::connectToChanged(QComboBox* box)
{
    connect(box, static_cast<void(QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged),
            this, &LaunchConfigurationPage::changed);
}

void IConfigPage::connectToChanged(QLineEdit* edit)
{
    connect(edit, &QLineEdit::textChanged, this, &LaunchConfigurationPage::changed);
}

void IConfigPage::connectToChanged(QSpinBox* box)
{
    connect(box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &LaunchConfigurationPage::changed);
}

}
