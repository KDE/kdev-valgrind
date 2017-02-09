/* This file is part of KDevelop
   Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING. If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "model.h"

#include <QAbstractTableModel>

namespace Valgrind
{

QString displayValue(int value)
{
    QString result = QString::number(value);
    int length = result.length();

    for (int i = 0; i < (length / 3); ++i) {
        int pos = result.length() - (4 * i + 3);
        if (!pos) {
            break;
        }
        result.insert(pos, ' ');
    }

    return result;
}

QString displayValue(double value)
{
    return QString::number(value, 'f', 2);
}

void emitDataChanged(QAbstractTableModel* model)
{
    Q_ASSERT(model);

    emit model->dataChanged(model->index(0,0),
                            model->index(model->rowCount() - 1, model->columnCount() - 1),
                            { Qt::DisplayRole });
}

}
