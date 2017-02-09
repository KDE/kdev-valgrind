/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2015 Laszlo Kis-Adam <laszlo.kis-adam@kdemail.net>
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

#include "parser.h"

#include "debug.h"
#include "error.h"

#include <klocalizedstring.h>
#include <kmessagebox.h>

#include <QApplication>

namespace valgrind
{

namespace Memcheck
{

Parser::Parser()
    : m_error(new Memcheck::Error)
{
}

Parser::~Parser()
{
    delete m_error;
}

void Parser::clear()
{
    m_stateStack.clear();
    m_name.clear();
    m_value.clear();
}

void Parser::startElement()
{
    State newState = Unknown;

    if (m_name == QStringLiteral("valgrindoutput")) {
        newState = Session;
    }

    else if (m_name == QStringLiteral("status")) {
        newState = Status;
    }

    else if (m_name == QStringLiteral("preamble")) {
        newState = Preamble;
    }

    else if (m_name == QStringLiteral("error")) {
        newState = Error;
        m_error->clear();
    }

    else if (m_name == QStringLiteral("stack")) {
        newState = Stack;
        m_error->addStack();
    }

    else if (m_name == QStringLiteral("frame")) {
        newState = Frame;
        m_error->lastStack().addFrame();
    }

    else {
        m_stateStack.push(m_stateStack.top());
        return;
    }

    m_stateStack.push(newState);
    return;
}

void Parser::endElement(QVector<KDevelop::IProblem::Ptr>& problems, bool showInstructionPointer)
{
    State state = m_stateStack.pop();

    switch (state) {

    case Error:
        m_error->setValue(m_name, m_value);

        if (m_name == QStringLiteral("error")) {
            problems.append(m_error->toIProblem(showInstructionPointer));
        }
        break;

    case Stack:
        m_error->lastStack().setValue(m_name, m_value);
        break;

    case Frame:
        m_error->lastStack().lastFrame().setValue(m_name, m_value);
        break;

    default:
        break;
    }
}

QVector<KDevelop::IProblem::Ptr> Parser::parse(bool showInstructionPointer)
{
    QVector<KDevelop::IProblem::Ptr> problems;

    while (!atEnd()) {
        switch (readNext()) {

        case StartDocument:
            clear();
            break;

        case StartElement:
            m_name = name().toString();
            m_value.clear();
            startElement();
            break;

        case EndElement:
            m_name = name().toString();
            endElement(problems, showInstructionPointer);
            break;

        case Characters:
            m_value += text();
            break;

        default:
            break;
        }
    }

    if (hasError()) {
        switch (error()) {

        case CustomError:
        case UnexpectedElementError:
        case NotWellFormedError:
            KMessageBox::error(qApp->activeWindow(),
                               i18n("Valgrind XML Parsing: error at line %1, column %2: %3",
                                    lineNumber(),
                                    columnNumber(),
                                    errorString()),
                               i18n("Valgrind Error"));
            break;

        case NoError:
        case PrematureEndOfDocumentError:
            break;
        }
    }

    return problems;
}

}

}
