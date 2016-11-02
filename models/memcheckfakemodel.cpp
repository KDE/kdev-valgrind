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

#include "memcheckfakemodel.h"

#include "memcheckitemsimpl.h"

#include <interfaces/icore.h>
#include <interfaces/ilanguagecontroller.h>
#include <shell/problemmodel.h>
#include <shell/problemmodelset.h>

namespace valgrind
{

class ValgrindProblem : public KDevelop::DetectedProblem
{
public:
    ValgrindProblem() {}
    ~ValgrindProblem() override {}

    QString sourceString() const override { return QStringLiteral("Valgrind"); };
};

MemcheckFakeModel::MemcheckFakeModel()
    : m_model(nullptr)
{
    KDevelop::ProblemModelSet *modelSet = KDevelop::ICore::self()->languageController()->problemModelSet();
    m_model = modelSet->findModel(QStringLiteral("Valgrind"));
    Q_ASSERT(m_model);

    m_model->clearProblems();
    m_problems.clear();
}

MemcheckFakeModel::~MemcheckFakeModel()
{
}

void MemcheckFakeModel::newElement(eElementType e)
{
    switch (e) {

    case startError:
        newStartError();
        break;

    case startStack:
        newStack();
        break;

    case startFrame:
        newFrame();
        break;

    default:
        break;
    }
}

void MemcheckFakeModel::newStack()
{
    m_errors.back()->addStack();
}

void MemcheckFakeModel::newStartError()
{
    m_errors << new MemcheckError();
}


void MemcheckFakeModel::newFrame()
{
    m_errors.back()->lastStack()->addFrame();
}


void MemcheckFakeModel::newData(eElementType e, const QString& name, const QString& value)
{
    switch (e) {

    case error:
    {
        MemcheckError* error = m_errors.back();
        error->incomingData(name, value);

        // End of the error
        if (name == QLatin1String("error")) {
            storeError();
            m_errors.clear();
            delete error;
        }

        break;
    }

    case frame:
        m_errors.back()->lastStack()->lastFrame()->incomingData(name, value);
        break;

    case stack:
        m_errors.back()->lastStack()->incomingData(name, value);
        break;

    default:
        break;
    }
}

// Builds a problem from the frame
KDevelop::IProblem::Ptr frameToProblem(const MemcheckFrame* frame)
{
    KDevelop::IProblem::Ptr frameProblem(new ValgrindProblem);

    KDevelop::DocumentRange range;
    range.setBothLines(frame->line - 1);

    // Don't set the document if the filename or directory is empty
    if (!frame->file.isEmpty() && !frame->dir.isEmpty()) {
        QString file = frame->dir + QStringLiteral("/") + frame->file;
        range.document = KDevelop::IndexedString(file);
    }

    frameProblem->setFinalLocation(range);
    frameProblem->setFinalLocationMode(KDevelop::IProblem::TrimmedLine);

    frameProblem->setDescription(frame->fn);
    frameProblem->setSource(KDevelop::IProblem::Plugin);

    return frameProblem;
}

// Builds a problem (with frames as diagnostics) from the stack
KDevelop::IProblem::Ptr stackToProblem(const MemcheckStack* stack)
{
    KDevelop::IProblem::Ptr stackProblem(new ValgrindProblem);
    stackProblem->setSource(KDevelop::IProblem::Plugin);

    foreach (const MemcheckFrame* frame, stack->getFrames()) {
        stackProblem->addDiagnostic(frameToProblem(frame));
    }

    // Find a file/line pair for the stack
    // It's tricky because not all frames have such a pair (library calls for example)
    if (!stackProblem->diagnostics().isEmpty()) {
        KDevelop::DocumentRange range;

        foreach (const KDevelop::IProblem::Ptr frameProblem, stackProblem->diagnostics()) {
            range = frameProblem->finalLocation();
            if (!range.document.isEmpty())
                break;
        }

        if (!range.document.isEmpty()) {
            stackProblem->setFinalLocation(range);
            stackProblem->setFinalLocationMode(KDevelop::IProblem::TrimmedLine);
        }
    }

    return stackProblem;
}

void MemcheckFakeModel::storeError()
{
    MemcheckError *error = m_errors.back();
    QString what = error->what;
    if (what.isEmpty())
        what = error->text;

    KDevelop::IProblem::Ptr problem(new ValgrindProblem);
    problem->setSource(KDevelop::IProblem::Plugin);
    problem->setDescription(what);

    // Add the stacks
    foreach (const MemcheckStack *stack, error->getStack()) {
        problem->addDiagnostic(stackToProblem(stack));
    }

    // First stack gets the 'what' description.
    // This stack is the one that shows the actual error
    // Hence why the problem gets it's file/line pair from here
    if (problem->diagnostics().size() >= 1) {
        KDevelop::IProblem::Ptr stackProblem = problem->diagnostics().at(0);
        stackProblem->setDescription(what);

        problem->setFinalLocation(stackProblem->finalLocation());
        problem->setFinalLocationMode(KDevelop::IProblem::TrimmedLine);
    }

    // The second stack is an auxilliary stack, it helps diagnose the problem
    // For example in case of an invalid read/write it shows where the deletion occured
    if (problem->diagnostics().size() >= 2) {
        KDevelop::IProblem::Ptr stackProblem = problem->diagnostics().at(1);
        stackProblem->setDescription(error->auxWhat);
    }

    m_problems.append(problem);
    m_model->setProblems(m_problems);
}

}
