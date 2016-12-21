/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2009 Andreas Pakulat <apaku@gmx.de>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2016 Anton Anikin <anton.anikin@htower.ru>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#pragma once

#include <interfaces/ilaunchmode.h>

#include <QMap>

class QIcon;

namespace valgrind
{

class LaunchMode : public KDevelop::ILaunchMode
{
public:
    virtual QString tool() const = 0;
};

class GenericLaunchMode : public LaunchMode
{
public:
    GenericLaunchMode();

    QIcon icon() const override;
    QString id() const override;
    QString name() const override;
    QString tool() const override;

protected:
    QString m_toolname;
};

}
