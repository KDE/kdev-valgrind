/* This file is part of KDevelop
 * Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
 * Copyright 2017 Anton Anikin <anton@anikin.xyz>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "massif_configpage.h"
#include "ui_massif_configpage.h"

#include "massif_settings.h"
#include "massif_tool.h"

#include <KConfigGroup>

namespace Valgrind
{

MassifConfigPage::MassifConfigPage(QWidget* parent)
    : ConfigPage(parent)
    , ui(new Ui::MassifConfigPage())
{
    ui->setupUi(this);

    connectToChanged(ui->extraParameters);
    connectToChanged(ui->launchMassifVisualizer);
    connectToChanged(ui->depth);
    connectToChanged(ui->threshold);
    connectToChanged(ui->peakInaccuracy);
    connectToChanged(ui->maxSnapshots);
    connectToChanged(ui->snapshotFreq);
    connectToChanged(ui->timeUnit);
    connectToChanged(ui->profileHeap);
    connectToChanged(ui->profileStack);
    connectToChanged(ui->pagesAsHeap);

    connectToChanged(ui->profileStack);
    connectToChanged(ui->pagesAsHeap);
    check();

    ui->timeUnitLabel->setToolTip(ui->timeUnit->toolTip());
    ui->depthLabel->setToolTip(ui->depth->toolTip());
    ui->thresholdLabel->setToolTip(ui->threshold->toolTip());
    ui->peakInaccuracyLabel->setToolTip(ui->peakInaccuracy->toolTip());
    ui->maxSnapshotsLabel->setToolTip(ui->maxSnapshots->toolTip());
    ui->snapshotFreqLabel->setToolTip(ui->snapshotFreq->toolTip());
}

MassifConfigPage::~MassifConfigPage() = default;

QString MassifConfigPage::title() const
{
    return MassifTool::self()->name();
}

QIcon MassifConfigPage::icon() const
{
    return QIcon();
}

void MassifConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    QSignalBlocker blocker(this);
    MassifSettings settings;
    settings.load(cfg);

    ui->extraParameters->setText(settings.extraParameters);
    ui->depth->setValue(settings.snapshotTreeDepth);
    ui->threshold->setValue(settings.threshold);
    ui->peakInaccuracy->setValue(settings.peakInaccuracy);
    ui->maxSnapshots->setValue(settings.maximumSnapshots);
    ui->snapshotFreq->setValue(settings.detailedSnapshotsFrequency);
    ui->timeUnit->setCurrentIndex(settings.timeUnit);
    ui->profileHeap->setChecked(settings.profileHeap);
    ui->profileStack->setChecked(settings.profileStack);
    ui->pagesAsHeap->setChecked(settings.pagesAsHeap);
    ui->launchMassifVisualizer->setChecked(settings.launchVisualizer);
}

void MassifConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    MassifSettings settings;

    settings.extraParameters = ui->extraParameters->text();
    settings.snapshotTreeDepth = ui->depth->value();
    settings.threshold = ui->threshold->value();
    settings.peakInaccuracy = ui->peakInaccuracy->value();
    settings.maximumSnapshots = ui->maxSnapshots->value();
    settings.detailedSnapshotsFrequency = ui->snapshotFreq->value();
    settings.timeUnit = ui->timeUnit->currentIndex();
    settings.profileHeap = ui->profileHeap->isChecked();
    settings.profileStack = ui->profileStack->isChecked();
    settings.pagesAsHeap = ui->pagesAsHeap->isChecked();
    settings.launchVisualizer = ui->launchMassifVisualizer->isChecked();

    settings.save(cfg);
}

void MassifConfigPage::check()
{
    if (ui->profileStack->isChecked() && ui->pagesAsHeap->isChecked()) {
        ui->messageWidget->setVisible(true);
        return;
    }

    ui->messageWidget->setVisible(false);
}

MassifConfigPageFactory::MassifConfigPageFactory()
{
}

KDevelop::LaunchConfigurationPage* MassifConfigPageFactory::createWidget(QWidget* parent)
{
    return new MassifConfigPage(parent);
}

}
