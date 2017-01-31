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

#include "modelitem.h"

#include "debug.h"


///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

namespace valgrind
{

void MemcheckError::addStack()
{
    stacks.append(new MemcheckStack);
}

void MemcheckError::setValue(const QString& name, const QString& value)
{
    if (name == "what")
        this->what = value;
    else if (name == "text")
        this->text = value;
    else if (name == "auxwhat")
        this->auxWhat = value;
}

void MemcheckStack::setValue(const QString& name, const QString& value)
{
    Q_UNUSED(value)
    if (name == "frame") {
        qCDebug(KDEV_VALGRIND) << "MemcheckStack::incomingData() Incoming data with frame name error";
    }
}


void MemcheckStack::addFrame()
{
    frames.append(new MemcheckFrame);
}

void MemcheckFrame::setValue(const QString& name, const QString& value)
{
    if (name == "ip")
        instructionPointer = value;

    else if (name == "obj")
        objectFile = value;

    else if (name == "fn")
        function = value;

    else if (name == "dir")
        directory = value;

    else if (name == "file")
        file = value;

    else if (name == "line")
        line = value.toInt();
}

QString MemcheckFrame::location() const
{
    if (directory.isEmpty() && file.isEmpty())
        return objectFile;

    return directory + "/" + file;
}

}
