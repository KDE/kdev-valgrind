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
#include <QHash>
#include <QList>
#include <QStack>
#include <QSortFilterProxyModel>
#include <QStringList>

#include "memcheckmodel.h"

namespace valgrind
{
class MemcheckError;
class MemcheckStack;
class MemcheckFrame;

class MemcheckError : public MemcheckItem
{

public:

    MemcheckError(valgrind::MemcheckModel* parent);

    virtual ~MemcheckError();

    virtual MemcheckModel* parent() const;

    MemcheckStack *addStack();

    MemcheckStack *lastStack() const;

    const QList<MemcheckStack *> &getStack() const;

    virtual void incomingData(QString name, QString value);

    void setKind(const QString& s);

    int uniqueId;
    int threadId;

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
    } m_kind;

    QString what, auxWhat, text;
    int leakedBytes, leakedBlocks;

private:
    QList<MemcheckStack *> m_stack;
    MemcheckModel* m_parent;
};

class MemcheckStack : public MemcheckItem
{
public:
    MemcheckStack(MemcheckError *parent);

    virtual ~MemcheckStack();

    virtual MemcheckError* parent() const;

    virtual void incomingData(QString name, QString value);

    QString what() const;

    MemcheckFrame *addFrame();

    MemcheckFrame *lastFrame() const;

    const QList<MemcheckFrame *> &getFrames() const;

private:
    QList<MemcheckFrame*> m_frames;
    MemcheckError* m_parent;
};

/**
 * A frame describes the location of a notification
 */
class MemcheckFrame : public MemcheckItem
{
public:

    /**
     * Takes a pointer on the parent stack
     */
    MemcheckFrame(MemcheckStack* parent);

    virtual MemcheckStack* parent() const;

    virtual void incomingData(QString name, QString value);

    QUrl url() const;

    int instructionPointer, line;
    QString obj, fn, dir, file;
    MemcheckStack* m_parent;
};
}

#endif /* _VALGRINDITEMIMPL_H */
