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

#include "valgrindparser.h"


ValgrindParser::ValgrindParser(QObject *parent)
{
  Q_UNUSED(parent)
}

ValgrindParser::~ValgrindParser()
{
}

void ValgrindParser::clear( )
{
    m_state = Root;
    m_stateStack.clear();
    m_buffer.clear();
    valgrindArgs.clear();
    programArgs.clear();

    // Send rest signal to model
    //    emit reset();
    // qDeleteAll(errors);
    // errors.clear();
    // reset();
}

bool ValgrindParser::startElement()
{
    m_buffer.clear();
    State newState = Unknown;

    if (name() == "valgrindoutput")
	newState = Session;
    else if (name() == "status")
	newState = Status;
    else if (name() == "preamble")
	newState = Preamble;
    else if (name() == "error") {
	newState = Error;
	emit newElement(ValgrindModel::startError);
    }
    else if (name() == "stack") {
	newState = Stack;
	emit newElement(ValgrindModel::startStack);
    }
    else if (name() == "frame") {
	newState = Frame;
	emit newElement(ValgrindModel::startFrame);
    }
    m_stateStack.push(m_state);
    m_state = newState;
    return true;
}

bool ValgrindParser::endElement()
{
    m_state = m_stateStack.pop();
    switch (m_state) {
    case Error:
	emit newData(ValgrindModel::error, name().toString(), m_buffer);
	break;
    case Stack:
	emit newData(ValgrindModel::stack, name().toString(), m_buffer);
	break;
    case Frame:
	emit newData(ValgrindModel::frame, name().toString(), m_buffer);
	break;
    default:
	break;
    }
    return true;
}

void ValgrindParser::parse()
{
    while (!atEnd()) {
        switch (readNext()) {
	case StartDocument:
	    clear();
	    break;
	case StartElement:
	    startElement();
	    break;
	case EndElement:
	    endElement();
	    break;
	case Characters:
	    m_buffer += text().toString();
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
	    KMessageBox::error(qApp->activeWindow(), i18n("Valgrind XML Parsing: error at line %1, column %2: %3", lineNumber(), columnNumber(), errorString()), i18n("Valgrind Error"));
	    break;
	case NoError:
	case PrematureEndOfDocumentError:
	    break;
        }
    }
}

#include "valgrindparser.moc"
