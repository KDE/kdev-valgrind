/* This file is part of KDevelop
 * Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>

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

#include "valgrinditem.h"
#include "memcheckmodel.h"

#include <kdebug.h>

///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

ValgrindError::ValgrindError(ValgrindModel* parent)
    : m_parent(parent)
{
}

ValgrindError::~ValgrindError()
{
}

ValgrindModel* ValgrindError::parent() const
{
    return m_parent;
}

void ValgrindError::incomingData(QString name, QString value)
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

void ValgrindError::setKind(const QString& s)
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


ValgrindStack *ValgrindError::addStack()
{
    m_stack << new ValgrindStack(this);
    return m_stack.back();
}

ValgrindStack *ValgrindError::lastStack() const
{
    return m_stack.back();
}

const QList<ValgrindStack *> &ValgrindError::getStack() const
{
    return m_stack;
}

////////////////////////

QString ValgrindStack::what() const
{
    return "In valgrindstack what";
}

ValgrindStack::ValgrindStack(ValgrindError *parent)
    : m_parent(parent)
{
}


ValgrindStack::~ValgrindStack()
{
}

ValgrindError* ValgrindStack::parent() const
{
    return m_parent;
}

void ValgrindStack::incomingData(QString name, QString value)
{
    Q_UNUSED(value)
    if (name == "frame") {
	qDebug() << "ValgrindStack::incomingData() Imcoming data with frame name error";
// if (this == this->parent()->stack)
	//     m_model->insertIntoTree(ValgrindModel::stack);
	//this->frames.append(m_model->m_frame);
    }
}


ValgrindFrame *ValgrindStack::addFrame()
{
    m_frames << new ValgrindFrame(this);
    return m_frames.back();
}

ValgrindFrame *ValgrindStack::lastFrame() const
{
    return m_frames.back();
}

const QList<ValgrindFrame *> &ValgrindStack::getFrames() const
{
    return m_frames;
}

ValgrindFrame::ValgrindFrame(ValgrindStack* parent)
    : m_parent(parent)
{
}

ValgrindStack* ValgrindFrame::parent() const
{
    return m_parent;
}

void ValgrindFrame::incomingData(QString name, QString value)
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

KUrl ValgrindFrame::url() const
{
    if (dir.isEmpty() && file.isEmpty())
        return KUrl();

    KUrl base = KUrl::fromPath(dir);
    base.adjustPath(KUrl::AddTrailingSlash);
    KUrl url(base, file);
    url.cleanPath();
    return url;
}
