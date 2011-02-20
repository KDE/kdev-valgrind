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

#ifndef _VALGRINDITEM_H_
#define _VALGRINDITEM_H_

#include <QHash>
#include <QStack>
#include <QSortFilterProxyModel>
#include <QStringList>

#include <KUrl>
#include "ivalgrinditem.h"
#include "valgrindmodel.h"

class ValgrindError;
class ValgrindFrame;
class ValgrindStack;

class ValgrindError : public ValgrindItem
{
public:
    ValgrindError(ValgrindModel* parent);

    virtual ~ValgrindError();

    virtual ValgrindModel* parent() const;

    virtual void incomingData(QString name, QString value);

    void setKind(const QString& s);

    QString whatForStack(const ValgrindStack* stack) const;

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
    } kind;

    QString what, auxWhat;
    int leakedBytes, leakedBlocks;

    ValgrindStack* stack;
    ValgrindStack* auxStack;
    ValgrindModel* m_parent;
};

class ValgrindStack : public ValgrindItem
{
 public:
    ValgrindStack(ValgrindModel *model, ValgrindError* parent);

    virtual ~ValgrindStack();

    virtual ValgrindError* parent() const { return m_parent; }

    virtual void incomingData(QString name, QString value);

    QString what() const;

    QList<ValgrindFrame*> frames;
    ValgrindModel* m_model;
    ValgrindError* m_parent;
};

class ValgrindFrame : public ValgrindItem
{
 public:
    ValgrindFrame(ValgrindStack* parent);

    virtual ValgrindStack* parent() const { return m_parent; }

    virtual void incomingData(QString name, QString value);

    KUrl url() const;

    int instructionPointer, line;
    QString obj, fn, dir, file;
    ValgrindStack* m_parent;
};

#endif /* _VALGRINDITEM_H_ */
