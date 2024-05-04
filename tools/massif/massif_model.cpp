/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
 * Copyright 2017 Anton Anikin <anton@anikin.xyz>

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

#include "massif_model.h"

#include "debug.h"
#include "massif_snapshot.h"

#include <KLocalizedString>

#include <QFontDatabase>

namespace Valgrind
{

MassifSnapshotsModel::MassifSnapshotsModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

MassifSnapshotsModel::~MassifSnapshotsModel()
{
    qDeleteAll(m_snapshots);
}

void MassifSnapshotsModel::addSnapshot(MassifSnapshot* snapshot)
{
    Q_ASSERT(snapshot);
    if (snapshot) {
        m_snapshots.append(snapshot);
    }
}

QModelIndex MassifSnapshotsModel::index(int row, int column, const QModelIndex&) const
{
    if (row >= 0 && row < rowCount() && column >= 0 && column < columnCount()) {
        return createIndex(row, column, m_snapshots.at(row));
    }

    return QModelIndex();
}

int MassifSnapshotsModel::rowCount(const QModelIndex&) const
{
    return m_snapshots.size();
}

int MassifSnapshotsModel::columnCount(const QModelIndex&) const
{
    return 5;
}

QString humanSize(int byteSize)
{
    static const QStringList units{ QStringLiteral("KiB"),
        QStringLiteral("MiB"),
        QStringLiteral("GiB"),
        QStringLiteral("TiB") };

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

QVariant MassifSnapshotsModel::data(const QModelIndex& index, int role) const
{
    auto snapshot = static_cast<MassifSnapshot*>(index.internalPointer());
    if (!snapshot) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        if (index.column() <= MassifSnapshot::Time) {
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

QVariant MassifSnapshotsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)

    if (role == Qt::DisplayRole) {
        switch (section) {

        case MassifSnapshot::SnapshotId:
            return i18n("Snapshot");

        case MassifSnapshot::Time:
            return i18n("Time");

        case MassifSnapshot::Heap:
            return i18n("Heap");

        case MassifSnapshot::HeapExtra:
            return i18n("Heap (extra)");

        case MassifSnapshot::Stack:
            return i18n("Stack");

        }
    }

    return QVariant();
}

}
