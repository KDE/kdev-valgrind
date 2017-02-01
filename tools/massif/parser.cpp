/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
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

#include "parser.h"

#include "model.h"

#include <kmessagebox.h>
#include <QFile>

namespace valgrind
{

namespace MassifParser
{

void parse(const QString& fileName, MassifModel* model)
{
    Q_ASSERT(model);

    QFile data(fileName);
    QString m_buffer;
    QString m_workingDir;
    QStringList m_lst;
    MassifItem* m_item(nullptr);

    data.open(QIODevice::ReadOnly);
    while (!data.atEnd()) {
        m_buffer = data.readLine();

        if (m_buffer.startsWith(QChar('#')) ||
            m_buffer.startsWith(QStringLiteral("desc")) ||
            m_buffer.startsWith(QStringLiteral("time_unit"))) {

            continue; // skip comment and useless lines
        }

        if (m_buffer.startsWith(QStringLiteral("cmd"))) {
            m_workingDir = m_buffer.split(QChar(' '))[1].split(QStringLiteral("build"))[0];
            continue;
        }

        m_lst = m_buffer.split(QChar('='));
        if (m_lst[0] == QStringLiteral("snapshot")) { // new snapshot
            m_item = new MassifItem();
            m_item->incomingData(m_lst[0], m_lst[1].trimmed());
            continue;
        }

        m_item->incomingData(m_lst[0], m_lst[1].trimmed());
        if (m_lst[0] == QStringLiteral("heap_tree")) {
            if (m_lst[1].startsWith(QStringLiteral("peak")) ||
                m_lst[1].startsWith(QStringLiteral("detailed"))) {

                while (!m_buffer.startsWith(QChar('#')) && !data.atEnd()) {
                    m_buffer = data.readLine();
                    if (m_buffer.startsWith(QChar('#'))) {
                        break;
                    }

                    MassifItem *child = new MassifItem(true);
                    child->setParent(m_item);
                    child->incomingData(QStringLiteral("child"), m_buffer.trimmed(), m_workingDir);
                    m_item->appendChild(child);
                }
            }

            // this is the last info, send the item to the model
            model->newItem(m_item);
        }
    }

    model->newItem(nullptr);
}

}

}
