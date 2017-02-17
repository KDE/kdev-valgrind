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
#include "settings.h"
#include "snapshot.h"

#include <QStringListModel>
#include <QProcess>
#include <QTemporaryFile>

namespace Valgrind
{

namespace Massif
{

View::View(KConfigGroup config, QTemporaryFile* outputFile, SnapshotsModel* model, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::View)
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
        auto snapshot = static_cast<Snapshot*>(current.internalPointer());
        treesModel->setStringList(snapshot->heapTree);
    });

    connect(ui->launchVisualizerButton, &QPushButton::clicked,
            this, [this, outputFile]() {
                launchVisualizer(outputFile->fileName());
            });

    connect(m_visualizerProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, [this]() {
                ui->launchVisualizerButton->setEnabled(true);
            });

    Settings settings;
    settings.load(config);
    if (settings.launchVisualizer) {
        launchVisualizer(outputFile->fileName());
    }
}

View::~View()
{
    delete m_visualizerProcess;
    delete ui;
}

void View::launchVisualizer(const QString& outputFile)
{
    m_visualizerProcess->start(Settings::visualizerExecutablePath(), { outputFile });
    ui->launchVisualizerButton->setEnabled(false);
}

}

}
