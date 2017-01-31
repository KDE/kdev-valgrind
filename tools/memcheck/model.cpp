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

#include "model.h"

#include "modelitem.h"

#include <interfaces/icore.h>
#include <interfaces/ilanguagecontroller.h>
#include <shell/problemmodel.h>
#include <shell/problemmodelset.h>

namespace valgrind
{

MemcheckFakeModel::MemcheckFakeModel(QObject* parent)
    : IModel(parent)
    , m_model(nullptr)
{
    KDevelop::ProblemModelSet* modelSet = KDevelop::ICore::self()->languageController()->problemModelSet();
    m_model = modelSet->findModel(QStringLiteral("Valgrind"));
    Q_ASSERT(m_model);

    m_model->clearProblems();
    m_problems.clear();
}

MemcheckFakeModel::~MemcheckFakeModel()
{
}

QAbstractItemModel* MemcheckFakeModel::abstractItemModel(int)
{
    return nullptr;
}

void MemcheckFakeModel::newElement(eElementType e)
{
    switch (e) {

    case startError:
        m_errors << new MemcheckError();
        break;

    case startStack:
        m_errors.back()->addStack();
        break;

    case startFrame:
        m_errors.back()->stacks.last().addFrame();
        break;

    default:
        break;
    }
}

void MemcheckFakeModel::newData(eElementType e, const QString& name, const QString& value)
{
    switch (e) {

    case error:
    {
        MemcheckError* error = m_errors.back();
        error->setValue(name, value);

        // End of the error
        if (name == QLatin1String("error")) {
            storeError();
            m_errors.clear();
            delete error;
        }

        break;
    }

    case frame:
        m_errors.back()->stacks.last().frames.last().setValue(name, value);
        break;

    case stack:
        m_errors.back()->stacks.last().setValue(name, value);
        break;

    default:
        break;
    }
}

void MemcheckFakeModel::storeError()
{
    m_problems.append(m_errors.last()->toIProblem(showInstructionPointer));
    m_model->setProblems(m_problems);
}

}
