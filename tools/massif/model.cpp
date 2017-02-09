/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
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

#include "model.h"

#include "debug.h"
#include "snapshot.h"

#include <klocalizedstring.h>

#include <QFontDatabase>

namespace valgrind
{

namespace Massif
{

SnapshotsModel::SnapshotsModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

SnapshotsModel::~SnapshotsModel()
{
    qDeleteAll(m_snapshots);
}

void SnapshotsModel::addSnapshot(Snapshot* snapshot)
{
    Q_ASSERT(snapshot);
    if (snapshot) {
        m_snapshots.append(snapshot);
    }
}

QModelIndex SnapshotsModel::index(int row, int column, const QModelIndex&) const
{
    if (row >= 0 && row < rowCount() && column >= 0 && column < columnCount()) {
        return createIndex(row, column, m_snapshots.at(row));
    }

    return QModelIndex();
}

int SnapshotsModel::rowCount(const QModelIndex&) const
{
    return m_snapshots.size();
}

int SnapshotsModel::columnCount(const QModelIndex&) const
{
    return 5;
}

QString humanSize(int byteSize)
{
    static const QStringList units{ "KiB", "MiB", "GiB", "TiB" };

    if (byteSize < 1024) {
        return QString::number(byteSize);
    }

    float size = byteSize;
    QStringListIterator i(units);
    QString unit;

    while (size >= 1024.0 && i.hasNext()) {
        unit = i.next();
        size /= 1024.0;
    }

    return QStringLiteral("%1 %2").arg(size, 0, 'f', 1).arg(unit);
}

QVariant SnapshotsModel::data(const QModelIndex& index, int role) const
{
    auto snapshot = static_cast<Snapshot*>(index.internalPointer());
    if (!snapshot) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        if (index.column() <= Snapshot::Time) {
            return snapshot->values[index.column()];
        }

        return humanSize(snapshot->values[index.column()].toInt());
    }

    if (role == Qt::FontRole) {
        QFont f = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
        if (!snapshot->heapTree.isEmpty()) {
            f.setBold(true);
        }
        return f;
    }

    return QVariant();
}

QVariant SnapshotsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)

    if (role == Qt::DisplayRole) {
        switch (section) {

        case Snapshot::SnapshotId:
            return i18n("Snapshot");

        case Snapshot::Time:
            return i18n("Time");

        case Snapshot::Heap:
            return i18n("Heap");

        case Snapshot::HeapExtra:
            return i18n("Heap (extra)");

        case Snapshot::Stack:
            return i18n("Stack");

        }
    }

    return QVariant();
}

}

}
