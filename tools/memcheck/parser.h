/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2016-2017 Anton Anikin <anton.anikin@htower.ru>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#pragma once

#include <interfaces/iproblem.h>

#include <QStack>
#include <QXmlStreamReader>

namespace valgrind
{

namespace Memcheck
{

struct Error;

class Parser : public QXmlStreamReader
{
public:
    Parser();
    virtual ~Parser();

    QVector<KDevelop::IProblem::Ptr> parse(bool showInstructionPointer);

private:
    void startElement();
    void endElement(QVector<KDevelop::IProblem::Ptr>& problems, bool showInstructionPointer);

    void clear();

    enum State {
        Unknown,
        Root,
        Session,
        Status,
        Preamble,
        Error,
        Stack,
        Frame
    };

    QStack<State> m_stateStack;

    QString m_name;
    QString m_value;

    Memcheck::Error* m_error;
};

}

}
