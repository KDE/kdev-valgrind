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

struct MemcheckError;
struct MemcheckStack;
struct MemcheckFrame;

struct MemcheckError
{
    void addStack();
    void setValue(const QString& name, const QString& value);

    QList<MemcheckStack*> stacks;

    QString what;
    QString auxWhat;
    QString text;
};

struct MemcheckStack
{
    void addFrame();
    void setValue(const QString& name, const QString& value);

    QList<MemcheckFrame*> frames;
};

struct MemcheckFrame
{
    void setValue(const QString& name, const QString& value);
    QString location() const;

    int line = 0;

    QString function = QStringLiteral("???");
    QString instructionPointer;

    QString objectFile;

    QString directory;
    QString file;
};

}

#endif
