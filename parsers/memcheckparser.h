/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2016 Anton Anikin <anton.anikin@htower.ru>

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

#include "iparser.h"

#include <QStack>

namespace valgrind
{

/**
 * A class which parses valgrind's XML output
 * and emits signals when items are parsed
 */
class MemcheckParser : public Parser
{
    Q_OBJECT

public:
    MemcheckParser(QObject* parent = nullptr);
    ~MemcheckParser() override;

public slots:
    void parse() override;

private:
    // XML parsing
    bool endElement();
    bool startElement();
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
    QString m_buffer;
};

}
