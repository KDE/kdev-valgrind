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

#ifndef _VALGRINDITEMIMPL_H_
#define _VALGRINDITEMIMPL_H_

#include <QUrl>
#include <QList>

#include "interfaces/imodel.h"

namespace valgrind
{

class MemcheckError;
class MemcheckStack;
class MemcheckFrame;

class MemcheckItem : public ModelItem
{

public:
    ~MemcheckItem() override {}

    /*
     * Called when data related to the error has been parsed
     */
    virtual void incomingData(const QString& name, const QString& value) = 0;
};

class MemcheckError : public MemcheckItem
{

public:

    MemcheckError();
    ~MemcheckError() override;

    MemcheckStack* addStack();
    MemcheckStack* lastStack() const;

    const QList<MemcheckStack*> &getStack() const;

    void incomingData(const QString& name, const QString& value) override;

    void setKind(const QString& s);

    int uniqueId; // FIXME remove ?
    int threadId; // FIXME remove ?

    enum {
        Unknown,
        InvalidFree,
        MismatchedFree,
        InvalidRead,
        InvalidWrite,
        InvalidJump,
        Overlap,
        InvalidMemPool,
        UninitCondition,
        UninitValue,
        SyscallParam,
        ClientCheck,
        Leak_DefinitelyLost,
        Leak_IndirectlyLost,
        Leak_PossiblyLost,
        Leak_StillReachable
    } m_kind;  // FIXME remove ?

    QString what;
    QString auxWhat;
    QString text;

    int leakedBytes; // FIXME remove ?
    int leakedBlocks; // FIXME remove ?

private:
    QList<MemcheckStack*> m_stack;
};

class MemcheckStack : public MemcheckItem
{
public:
    MemcheckStack();
    ~MemcheckStack() override;

    void incomingData(const QString& name, const QString& value) override;

    QString what() const { return m_what; }
    void setWhat(const QString& what) { m_what = what; }

    MemcheckFrame* addFrame();
    MemcheckFrame* lastFrame() const;

    const QList<MemcheckFrame*> &getFrames() const;

private:
    QList<MemcheckFrame*> m_frames;

    // What is this stack about?
    QString m_what;
};

/**
 * A frame describes the location of a notification
 */
class MemcheckFrame : public MemcheckItem
{
public:
    MemcheckFrame();

    void incomingData(const QString& name, const QString& value) override;

    QUrl url() const;

    int line;

    QString instructionPointer;
    QString obj;
    QString fn;
    QString dir;
    QString file;
};

}

#endif
