/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#include <QString>
#include <QMap>
#include <QUrl>
#include <QVariant>

namespace valgrind
{

class MassifItem
{
public:
    explicit MassifItem(bool isChild = false);
    ~MassifItem();

    enum Columns {
        Snapshot,
        Time,
        MemHeapB,
        MemHeapExtraB,
        MemStacksB
    };

    void incomingData(const QString& name, const QString& value, const QString& dir = QStringLiteral(""));
    void incomingAlloc(const QString& value);

    MassifItem* parent();

    MassifItem* child(int row);
    void addChild(MassifItem* child);
    int childCount() const;

    int columnCount() const;
    QVariant data(int column) const;

    int row() const;
    QUrl url() const; // FIXME return QString
    int line() const;

private:
    MassifItem* m_parent;
    QList<MassifItem*> m_childs;

    bool m_isChild;

    QMap<QString, QString> m_values;

    QString m_dir;
    QString m_file;
    int m_line;

};

}
