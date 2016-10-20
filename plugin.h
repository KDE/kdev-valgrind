/* This file is part of KDevelop
   Copyright 2002 Harald Fernengel <harry@kdevelop.org>
   Copyright 2007 Hamish Rodda <rodda@kde.org>
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

#include <interfaces/iplugin.h>

namespace KDevelop
{
class ProblemModel;
}

namespace valgrind
{
class Marks;
class Model;
class WidgetFactory;

class Plugin : public KDevelop::IPlugin
{
    Q_OBJECT

public:
    Plugin(QObject* parent, const QVariantList& = QVariantList());
    ~Plugin() override;

    void incomingModel(Model* model);

signals:
    void newModel(Model* model);

private:
    void runValgrind();

    QString m_lastExec;
    QString m_lastParams;
    QString m_lastValExec;
    QString m_lastValParams;
    QString m_lastCtExec;
    QString m_lastCtParams;
    QString m_lastKcExec;

    WidgetFactory* m_factory;
    Marks* m_marks;

    QScopedPointer<KDevelop::ProblemModel> m_model;
};

}

