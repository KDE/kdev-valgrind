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

#include <QRegularExpression>

namespace Valgrind
{

struct Problem : public KDevelop::DetectedProblem
{
    ~Problem() override {}

    KDevelop::IProblem::Source source() const override { return KDevelop::IProblem::Plugin; }
    QString sourceString() const override { return QStringLiteral("Valgrind"); };
};

namespace Memcheck
{

void Frame::setValue(const QString& name, const QString& value)
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

KDevelop::IProblem::Ptr Frame::toIProblem(bool showInstructionPointer) const
{
    KDevelop::IProblem::Ptr frameProblem(new Problem);

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

void Stack::addFrame()
{
    frames.append(Frame{});
}

Frame& Stack::lastFrame()
{
    Q_ASSERT(frames.count());
    return frames.last();
}

void Stack::setValue(const QString& name, const QString& value)
{
    Q_UNUSED(value)

    if (name == "frame") {
        qCDebug(KDEV_VALGRIND) << "MemcheckStack::setValue() Incoming data with frame name error";
    }
}

KDevelop::IProblem::Ptr Stack::toIProblem(bool showInstructionPointer) const
{
    KDevelop::IProblem::Ptr stackProblem(new Problem);

    KDevelop::DocumentRange range(KDevelop::DocumentRange::invalid());
    foreach (const Frame& frame, frames) {
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

void Error::clear()
{
    stacks.clear();
    messages.clear();
}

void Error::addStack()
{
    stacks.append(Stack{});
}

Stack& Error::lastStack()
{
    Q_ASSERT(stacks.count());
    return stacks.last();
}

void Error::setValue(const QString& name, const QString& value)
{
    // Fix for memcheck messages
    static const QRegularExpression memcheckSuffix(" in loss record \\d+ of \\d+$");

    if (!value.isEmpty() && (name == QStringLiteral("text") ||
                             name == QStringLiteral("auxwhat") ||
                             name == QStringLiteral("what"))) {
        messages += value.trimmed().remove(memcheckSuffix);
    }
}

KDevelop::IProblem::Ptr Error::toIProblem(bool showInstructionPointer) const
{
    Q_ASSERT(!messages.isEmpty());
    Q_ASSERT(!stacks.isEmpty());
    Q_ASSERT(messages.size() >= stacks.size());

    // Simplify view for errors with single stack / message
    if (stacks.size() == 1 && messages.size() == 1) {
        auto problem = stacks.first().toIProblem(showInstructionPointer);
        problem->setDescription(messages.first());
        return problem;
    }

    KDevelop::IProblem::Ptr problem(new Problem);
    problem->setDescription(messages.first());

    // Add all stacks
    foreach (const Stack& stack, stacks) {
        problem->addDiagnostic(stack.toIProblem(showInstructionPointer));
    }

    // First stack is the one that shows the actual error
    // Hence why the problem gets it's file/line pair from here
    problem->setFinalLocation(problem->diagnostics().first()->finalLocation());
    problem->setFinalLocationMode(KDevelop::IProblem::TrimmedLine);

    // Set descriptions for all stacks. If we have some "extra" messages, then
    // we add them as "empty" (text-only) problems.
    for (int i = 0; i < messages.size(); ++i) {
        if (i < stacks.size()) {
            problem->diagnostics().at(i)->setDescription(messages.at(i));
        }

        else {
            KDevelop::IProblem::Ptr messageOnlyProblem(new Problem);
            messageOnlyProblem->setDescription(messages.at(i));
            messageOnlyProblem->setFinalLocation(problem->finalLocation());
            problem->addDiagnostic(messageOnlyProblem);
            qDebug() << messageOnlyProblem->finalLocation() << messageOnlyProblem->finalLocation().document;
        }
    }

    return problem;
}

}

}
