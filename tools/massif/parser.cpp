/* This file is part of KDevelop
   Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
   Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
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

#include "parser.h"

#include "model.h"
#include "snapshot.h"

#include <QFile>

namespace Valgrind
{

namespace Massif
{

void parse(const QString& fileName, SnapshotsModel* model)
{
    Q_ASSERT(model);

    Snapshot* snapshot = nullptr;
    QString line;
    QStringList keyValue;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    while (!file.atEnd()) {
        line = file.readLine();

        if (line.startsWith(QChar('#')) ||
            line.startsWith(QStringLiteral("desc")) ||
            line.startsWith(QStringLiteral("time_unit")) ||
            line.startsWith(QStringLiteral("cmd"))) {

            continue; // skip comment and useless lines
        }

        keyValue = line.split(QChar('='));
        const QString& key = keyValue.at(0);
        const QString& value = keyValue.at(1);

        if (key == QStringLiteral("snapshot")) {
            snapshot = new Snapshot;
            snapshot->setValue(key, value.trimmed());
            continue;
        }

        Q_ASSERT(snapshot);
        snapshot->setValue(key, value.trimmed());

        if (key == QStringLiteral("heap_tree")) {
            if (value.startsWith(QStringLiteral("peak")) ||
                value.startsWith(QStringLiteral("detailed"))) {

                while (!file.atEnd()) {
                    line = file.readLine();
                    if (line.startsWith(QChar('#'))) {
                        break;
                    }

                    snapshot->heapTree.append(line.remove(QLatin1Char('\n')));
                }
            }

            model->addSnapshot(snapshot);
        }
    }
}

}

}
