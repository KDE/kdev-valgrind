/* This file is part of KDevelop
 *  Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 *  Copyright 2008 Hamish Rodda <rodda@kde.org>
 *  Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
 *  Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#pragma once

#include <QWidget>
#include <kconfiggroup.h>

class QProcess;
class QTemporaryFile;

namespace Valgrind
{

namespace Callgrind
{

namespace Ui
{

class View;

}

class FunctionsModel;

class View : public QWidget
{
    Q_OBJECT

public:
    View(KConfigGroup config, QTemporaryFile* outputFile, FunctionsModel* model, QWidget* parent = nullptr);
    ~View() override;

private:
    Ui::View* ui;
    QProcess* m_kcachegrindProcess;
};

}

}
