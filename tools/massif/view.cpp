/* This file is part of KDevelop
 *  Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 *  Copyright 2008 Hamish Rodda <rodda@kde.org>
 *  Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#include "view.h"
#include "ui_view.h"

#include "debug.h"
#include "model.h"
#include "snapshot.h"

#include <QStringListModel>

namespace valgrind
{

MassifView::MassifView(MassifModel* model, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MassifView)
{
    Q_ASSERT(model);
    model->setParent(this);

    ui->setupUi(this);

    auto treesModel = new QStringListModel(this);
    ui->treesView->setModel(treesModel);

    ui->snapshotsView->setModel(model);
    ui->snapshotsView->header()->resizeSections(QHeaderView::ResizeToContents);

    connect(ui->snapshotsView->selectionModel(), &QItemSelectionModel::currentChanged, this,
            [treesModel](const QModelIndex& current, const QModelIndex&) {
        auto snapshot = static_cast<MassifSnapshot*>(current.internalPointer());
        treesModel->setStringList(snapshot->heapTree);
    });
}

MassifView::~MassifView()
{
    delete ui;
}

}
