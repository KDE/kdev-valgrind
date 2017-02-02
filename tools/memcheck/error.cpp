/* This file is part of KDevelop
 * Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
 * Copyright 2011 Sarie Lucas <lucas.sarie@gmail.com>
 * Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#include "error.h"

#include "debug.h"

#include <shell/problem.h>

namespace valgrind
{

struct ValgrindProblem : public KDevelop::DetectedProblem
{
    ~ValgrindProblem() override {}

    KDevelop::IProblem::Source source() const override { return KDevelop::IProblem::Plugin; }
    QString sourceString() const override { return QStringLiteral("Valgrind"); };
};

void MemcheckFrame::setValue(const QString& name, const QString& value)
{
    if (name == "ip") {
        instructionPointer = value;
    }

    else if (name == "obj") {
        objectFile = value;
    }

    else if (name == "fn") {
        function = value;
    }

    else if (name == "dir") {
        directory = value;
    }

    else if (name == "file") {
        file = value;
    }

    else if (name == "line") {
        line = value.toInt();
    }
}

KDevelop::IProblem::Ptr MemcheckFrame::toIProblem(bool showInstructionPointer) const
{
    KDevelop::IProblem::Ptr frameProblem(new ValgrindProblem);

    KDevelop::DocumentRange range;
    range.setBothLines(line - 1);

    if (directory.isEmpty() && file.isEmpty()) {
        range.document = KDevelop::IndexedString(objectFile);
    } else {
        range.document = KDevelop::IndexedString(directory + "/" + file);
    }

    frameProblem->setFinalLocation(range);
    frameProblem->setFinalLocationMode(KDevelop::IProblem::TrimmedLine);

    QString description;
    if (showInstructionPointer) {
        description = QStringLiteral("%1: ").arg(instructionPointer);
    }
    description += function;
    frameProblem->setDescription(description);

    return frameProblem;
}

void MemcheckStack::addFrame()
{
    frames.append(MemcheckFrame{});
}

MemcheckFrame& MemcheckStack::lastFrame()
{
    Q_ASSERT(frames.count());
    return frames.last();
}

void MemcheckStack::setValue(const QString& name, const QString& value)
{
    Q_UNUSED(value)

    if (name == "frame") {
        qCDebug(KDEV_VALGRIND) << "MemcheckStack::setValue() Incoming data with frame name error";
    }
}

KDevelop::IProblem::Ptr MemcheckStack::toIProblem(bool showInstructionPointer) const
{
    KDevelop::IProblem::Ptr stackProblem(new ValgrindProblem);

    KDevelop::DocumentRange range(KDevelop::DocumentRange::invalid());
    foreach (const MemcheckFrame& frame, frames) {
        auto frameProblem = frame.toIProblem(showInstructionPointer);
        stackProblem->addDiagnostic(frameProblem);

        if (!range.isValid() && !frame.file.isEmpty()) {
            range = frameProblem->finalLocation();
        }
    }

    stackProblem->setFinalLocation(range);
    stackProblem->setFinalLocationMode(KDevelop::IProblem::TrimmedLine);

    return stackProblem;
}

void MemcheckError::clear()
{
    what.clear();
    auxWhat.clear();
    text.clear();
    stacks.clear();
}

void MemcheckError::addStack()
{
    stacks.append(MemcheckStack{});
}

MemcheckStack& MemcheckError::lastStack()
{
    Q_ASSERT(stacks.count());
    return stacks.last();
}

void MemcheckError::setValue(const QString& name, const QString& value)
{
    if (name == "what") {
        this->what = value;
    }

    else if (name == "text") {
        this->text = value;
    }

    else if (name == "auxwhat") {
        this->auxWhat = value;
    }
}

KDevelop::IProblem::Ptr MemcheckError::toIProblem(bool showInstructionPointer) const
{
    KDevelop::IProblem::Ptr problem(new ValgrindProblem);

    if (what.isEmpty()) {
        problem->setDescription(text);
    } else {
        problem->setDescription(what);
    }

    // Add the stacks
    foreach (const MemcheckStack& stack, stacks) {
        problem->addDiagnostic(stack.toIProblem(showInstructionPointer));
    }

    // First stack gets the problem's description.
    // This stack is the one that shows the actual error
    // Hence why the problem gets it's file/line pair from here
    if (problem->diagnostics().size() >= 1) {
        KDevelop::IProblem::Ptr stackProblem = problem->diagnostics().at(0);
        stackProblem->setDescription(problem->description());

        problem->setFinalLocation(stackProblem->finalLocation());
        problem->setFinalLocationMode(KDevelop::IProblem::TrimmedLine);
    }

    // The second stack is an auxiliary stack, it helps diagnose the problem
    // For example in case of an invalid read/write it shows where the deletion happens
    if (problem->diagnostics().size() >= 2) {
        KDevelop::IProblem::Ptr stackProblem = problem->diagnostics().at(1);
        stackProblem->setDescription(auxWhat);
    }

    return problem;
}

}
