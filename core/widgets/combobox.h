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

#include <QComboBox>

namespace Valgrind
{

class ComboBox : public QComboBox
{
    Q_OBJECT

    Q_PROPERTY(
        QString value
        READ value
        WRITE setValue
        NOTIFY valueChanged
        USER true)

public:
    explicit ComboBox(QWidget* parent = nullptr);
    ~ComboBox() override = default;

    QString value() const;
    void setValue(const QString& value);

Q_SIGNALS:
    void valueChanged(const QString& value);
};

}
