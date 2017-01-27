/* This file is part of KDevelop
   Copyright 2007-2008 Hamish Rodda <rodda@kde.org>
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

#include <QTabWidget>

class QResizeEvent;

namespace valgrind
{
class IModel;
class Plugin;

class Widget : public QTabWidget
{
    Q_OBJECT

public:
    Widget(Plugin* plugin, QWidget* parent);

    Plugin* plugin() const;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void newModel(IModel* model);
    void destroyRequestedTab(int tab);

    Plugin* m_plugin;
};

}
