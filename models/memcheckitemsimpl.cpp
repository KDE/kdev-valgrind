/* This file is part of KDevelop
 * Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
 * Copyright 2011 Sarie Lucas <lucas.sarie@gmail.com>

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

#include "memcheckitemsimpl.h"

#include "debug.h"


///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

namespace valgrind
{

MemcheckError::MemcheckError()
{
}

MemcheckError::~MemcheckError()
{
}

void MemcheckError::incomingData(const QString& name, const QString& value)
{
    if (name == "unique")
        this->uniqueId = value.toInt(0L, 16);
    else if (name == "tid")
        this->threadId = value.toInt();
    else if (name == "kind")
        this->setKind(value);
    else if (name == "what")
        this->what = value;
    else if (name == "leakedbytes")
        this->leakedBytes = value.toInt();
    else if (name == "leakedblocks")
        this->leakedBlocks = value.toInt();
    else if (name == "text")
        this->text = value;
    else if (name == "auxwhat")
        this->auxWhat = value;
}

void MemcheckError::setKind(const QString& s)
{
    if (s == "Unknown")
        m_kind = Unknown;
    else if (s == "InvalidFree")
        m_kind = InvalidFree;
    else if (s == "MismatchedFree")
        m_kind = MismatchedFree;
    else if (s == "InvalidRead")
        m_kind = InvalidRead;
    else if (s == "InvalidWrite")
        m_kind = InvalidWrite;
    else if (s == "InvalidJump")
        m_kind = InvalidJump;
    else if (s == "Overlap")
        m_kind = Overlap;
    else if (s == "InvalidMemPool")
        m_kind = InvalidMemPool;
    else if (s == "UninitCondition")
        m_kind = UninitCondition;
    else if (s == "UninitValue")
        m_kind = UninitValue;
    else if (s == "SyscallParam")
        m_kind = SyscallParam;
    else if (s == "ClientCheck")
        m_kind = ClientCheck;
    else if (s == "Leak_DefinitelyLost")
        m_kind = Leak_DefinitelyLost;
    else if (s == "Leak_IndirectlyLost")
        m_kind = Leak_IndirectlyLost;
    else if (s == "Leak_PossiblyLost")
        m_kind = Leak_PossiblyLost;
    else if (s == "Leak_StillReachable")
        m_kind = Leak_StillReachable;
    else
        m_kind = Unknown;
}


MemcheckStack *MemcheckError::addStack()
{
    m_stack << new MemcheckStack(this);
    MemcheckStack *stack = m_stack.back();

    if (m_stack.count() == 1) {
        if (!this->what.isEmpty())
            stack->setWhat(this->what + " at:");
        else
            stack->setWhat(this->text + " at:");
    } else {
        stack->setWhat(this->auxWhat + " at:");
    }

    return stack;
}

MemcheckStack *MemcheckError::lastStack() const
{
    return m_stack.back();
}

const QList<MemcheckStack *> &MemcheckError::getStack() const
{
    return m_stack;
}

////////////////////////

MemcheckStack::MemcheckStack(MemcheckError *parent)
    : m_parent(parent)
{
}


MemcheckStack::~MemcheckStack()
{
}

MemcheckError* MemcheckStack::parent() const
{
    return m_parent;
}

void MemcheckStack::incomingData(const QString& name, const QString& value)
{
    Q_UNUSED(value)
    if (name == "frame") {
        qCDebug(KDEV_VALGRIND) << "MemcheckStack::incomingData() Imcoming data with frame name error";
    }
}


MemcheckFrame *MemcheckStack::addFrame()
{
    m_frames << new MemcheckFrame(this);
    return m_frames.back();
}

MemcheckFrame *MemcheckStack::lastFrame() const
{
    return m_frames.back();
}

const QList<MemcheckFrame *> &MemcheckStack::getFrames() const
{
    return m_frames;
}

MemcheckFrame::MemcheckFrame(MemcheckStack* parent)
    : m_parent(parent)
{
}

MemcheckStack* MemcheckFrame::parent() const
{
    return m_parent;
}

void MemcheckFrame::incomingData(const QString& name, const QString& value)
{
    if (name == "ip")
        this->instructionPointer = value.toInt(0L, 16);
    else if (name == "obj")
        this->obj = value;
    else if (name == "fn")
        this->fn = value;
    else if (name == "dir")
        this->dir = value;
    else if (name == "file")
        this->file = value;
    else if (name == "line")
        this->line = value.toInt();
}

QUrl MemcheckFrame::url() const
{
    if (dir.isEmpty() && file.isEmpty())
        return QUrl();

    QUrl base = QUrl::fromLocalFile(dir);
    base.setPath(base.path() + '/');
    QUrl url = base.resolved(QUrl(file));

    return url;
}

}
