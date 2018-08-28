/* This file is part of KDevelop
 *  Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 *  Copyright 2008 Hamish Rodda <rodda@kde.org>
 *  Copyright 2017 Anton Anikin <anton@anikin.xyz>

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

#include "massif_view.h"
#include "ui_massif_view.h"

#include "debug.h"
#include "massif_config.h"
#include "massif_model.h"
#include "massif_snapshot.h"
#include "utils.h"

#include <QStringListModel>
#include <QProcess>
#include <QTemporaryFile>

namespace Valgrind
{

MassifView::MassifView(KConfigGroup configGroup, QTemporaryFile* outputFile, MassifSnapshotsModel* model, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MassifView)
    , m_visualizerProcess(new QProcess)
{
    Q_ASSERT(model);
    model->setParent(this);

    Q_ASSERT(outputFile);
    outputFile->setParent(this);

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

    auto startVisualizer = [this, outputFile]() {
        m_visualizerProcess->start(MassifConfig::visualizerExecutablePath(),
                                   { outputFile->fileName() });
    };

    MassifConfig config;
    config.setConfigGroup(configGroup);
    config.load();

    setupVisualizerProcess(m_visualizerProcess.data(),
                           ui->launchVisualizerButton,
                           startVisualizer, config.launchVisualizer());
}

MassifView::~MassifView()
{
    m_visualizerProcess->disconnect(); // FIXME is this really needed ?
}

}
