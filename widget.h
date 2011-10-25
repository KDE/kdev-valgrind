/* This file is part of KDevelop
 *  Copyright 2007-2008 Hamish Rodda <rodda@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
 */

#ifndef VALGRINDWIDGET_H
#define VALGRINDWIDGET_H

#include <QTabWidget>

namespace valgrind
{
class Model;
class Plugin;
class Job;

class Widget : public QTabWidget
{
    Q_OBJECT

public:
    Widget(valgrind::Plugin* plugin, QWidget* parent);

    valgrind::Plugin* plugin() const;

public slots:
  void updateTabText(int index, const QString & text);

private Q_SLOTS:

    void newModel(valgrind::Model* model);
    void modelDestroyed(QObject* model);

private:
    valgrind::Plugin* m_plugin;
};
}

#endif // VALGRINDWIDGET_H
