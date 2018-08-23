/* This file is part of KDevelop
   Copyright 2006-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2015 Laszlo Kis-Adam <laszlo.kis-adam@kdemail.net>
   Copyright 2016-2017 Anton Anikin <anton@anikin.xyz>

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

#include "xmlparser.h"

#include "debug.h"
#include "xmlerror.h"
#include "utils.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QStack>
#include <QXmlStreamReader>

namespace Valgrind
{

namespace XmlParser
{

class Parser : public QXmlStreamReader
{
public:
    explicit Parser(const QString& toolName);
    virtual ~Parser();

    QVector<KDevelop::IProblem::Ptr> parse(bool showInstructionPointer);

private:
    void startElement();
    void endElement(QVector<KDevelop::IProblem::Ptr>& problems, bool showInstructionPointer);

    void clear();

    enum State {
        Unknown,
        Session,
        Status,
        Preamble,
        Error,
        Stack,
        Frame,

        // DRD tool
        OtherSegmentStart,
        OtherSegmentEnd
    };

    QString m_toolName;

    QStack<State> m_stateStack;

    QString m_name;
    QString m_value;

    XmlParser::Frame* m_frame;
    XmlParser::Stack* m_stack;
    XmlParser::OtherSegment* m_otherSegment;
    XmlParser::Error* m_error;
};

inline QString errorXmlName() { return QStringLiteral("error"); }
inline QString stackXmlName() { return QStringLiteral("stack"); }
inline QString frameXmlName() { return QStringLiteral("frame"); }

inline QString otherSegmentStartXmlName() { return QStringLiteral("other_segment_start"); }
inline QString otherSegmentEndXmlName()   { return QStringLiteral("other_segment_end"); }

Parser::Parser(const QString& toolName)
    : m_toolName(toolName)
    , m_frame(nullptr)
    , m_stack(nullptr)
    , m_otherSegment(nullptr)
    , m_error(new XmlParser::Error)
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

    else if (m_name == errorXmlName()) {
        newState = Error;
        m_error->clear();
    }

    else if (m_name == stackXmlName()) {
        newState = Stack;

        // Useful stacks are inside error or other_segment_begin/end
        if (m_stateStack.top() == Error) {
            m_stack = m_error->addStack();
        } else if (m_stateStack.top() == OtherSegmentStart ||
                   m_stateStack.top() == OtherSegmentEnd) {

            Q_ASSERT(m_otherSegment);
            m_stack = m_otherSegment->addStack();
        }
    }

    else if (m_name == frameXmlName()) {
        newState = Frame;
        if (m_stack) {
            m_frame = m_stack->addFrame();
        }
    }

    else if (m_name == otherSegmentStartXmlName()) {
        newState = OtherSegmentStart;
        m_otherSegment = m_error->addOtherSegment(true);
    }

    else if (m_name == otherSegmentEndXmlName()) {
        newState = OtherSegmentEnd;
        m_otherSegment = m_error->addOtherSegment(false);
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
        if (m_name == errorXmlName()) {
            problems.append(m_error->toIProblem(m_toolName, showInstructionPointer));
        } else {
            m_error->setValue(m_name, m_value);
        }
        break;

    case Stack:
        if (m_stack && m_name == stackXmlName()) {
            m_stack = nullptr;
        }
        break;

    case Frame:
        if (m_frame) {
            if (m_name == frameXmlName()) {
                m_frame = nullptr;
            } else {
                m_frame->setValue(m_name, m_value);
            }
        }
        break;

    case OtherSegmentStart:
        if (m_name == otherSegmentStartXmlName()) {
            m_otherSegment = nullptr;
        }
        break;

    case OtherSegmentEnd:
        if (m_name == otherSegmentEndXmlName()) {
            m_otherSegment = nullptr;
        }
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
            KMessageBox::error(activeMainWindow(),
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

QVector<KDevelop::IProblem::Ptr> parseXml(const QString& toolName, const QString& xmlData, bool showInstructionPointer)
{
    XmlParser::Parser parser(toolName);
    parser.addData(xmlData);

    return parser.parse(showInstructionPointer);
}

}
