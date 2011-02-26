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
#include "valgrindmodel.h"

#include <kdebug.h>

ValgrindError::ValgrindError(ValgrindModel* parent)
    : uniqueId(-1)
    , threadId(-1)
    , kind(Unknown)
    , leakedBytes(0)
    , leakedBlocks(0)
    , stack(0L)
    , auxStack(0L)
    , m_parent(parent)
{
}

ValgrindError::~ ValgrindError( )
{
    delete stack;
    delete auxStack;
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
    else if (name == "auxwhat")
	this->auxWhat = value;
    else if (name == "stack") {
	if (!this->stack) {
	    this->stack = this->m_parent->m_stack;
	} else if (!this->auxStack) {
	    m_parent->insertIntoTree(ValgrindModel::error);
	} else {
	    delete this->m_parent->m_stack;
	    kWarning() << "Unexpected stack received";
	}
    }
}

QString ValgrindError::whatForStack(const ValgrindStack * s) const
{
    if (s == stack)
        return what;
    if (s == auxStack)
        return auxWhat;
    return "<INTERNAL ERROR>";
}


void ValgrindError::setKind(const QString& s)
{
    if (s == "Unknown")
        kind = Unknown;
    else if (s == "InvalidFree")
        kind = InvalidFree;
    else if (s == "MismatchedFree")
        kind = MismatchedFree;
    else if (s == "InvalidRead")
        kind = InvalidRead;
    else if (s == "InvalidWrite")
        kind = InvalidWrite;
    else if (s == "InvalidJump")
        kind = InvalidJump;
    else if (s == "Overlap")
        kind = Overlap;
    else if (s == "InvalidMemPool")
        kind = InvalidMemPool;
    else if (s == "UninitCondition")
        kind = UninitCondition;
    else if (s == "UninitValue")
        kind = UninitValue;
    else if (s == "SyscallParam")
        kind = SyscallParam;
    else if (s == "ClientCheck")
        kind = ClientCheck;
    else if (s == "Leak_DefinitelyLost")
        kind = Leak_DefinitelyLost;
    else if (s == "Leak_IndirectlyLost")
        kind = Leak_IndirectlyLost;
    else if (s == "Leak_PossiblyLost")
        kind = Leak_PossiblyLost;
    else if (s == "Leak_StillReachable")
        kind = Leak_StillReachable;
    else
        kind = Unknown;
}

/*-----------------------------------------------*/

ValgrindFrame::ValgrindFrame(ValgrindStack* parent)
    : instructionPointer(0)
    , line(-1)
    , m_parent(parent)
{
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

/*-----------------------------------------------*/

ValgrindStack::ValgrindStack(ValgrindModel* model , ValgrindError *parent)
    : m_model(model)
    , m_parent(parent)
{
}

ValgrindStack::~ ValgrindStack( )
{
    qDeleteAll(frames);
}

void ValgrindStack::incomingData(QString name, QString value)
{
    if (name == "frame") {
	if (this == this->parent()->stack)
	    m_model->insertIntoTree(ValgrindModel::stack);
	this->frames.append(m_model->m_frame);
    }
}


QString ValgrindStack::what() const
{
    return parent()->whatForStack(this);
}



