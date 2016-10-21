/* This file is part of KDevelop
   Copyright 2015 Laszlo Kis-Adam <laszlo.kis-adam@kdemail.net>
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

#include "imodel.h"

#include <interfaces/iproblem.h>

#include <QList>

namespace KDevelop
{

class ProblemModel;

}

namespace valgrind
{

class MemcheckError;

// A fake model, all it does is creates problem instances and sends them to ProblemModel
class MemcheckFakeModel : public Model
{
public:
    MemcheckFakeModel();
    ~MemcheckFakeModel() override;

    void newStack();
    void newFrame();
    void newStartError();

    virtual void newElement(Model::eElementType) override;
    virtual void newData(Model::eElementType, const QString &name, const QString &value) override;

private:
    // Converts the error to a problem and stores in ProblemModel
    void storeError();

    QList<MemcheckError*> m_errors;
    KDevelop::ProblemModel* m_model;
    QVector<KDevelop::IProblem::Ptr> m_problems;
};

}

