/* This file is part of KDevelop
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


#include "drd_configpage.h"
#include "ui_drd_configpage.h"

#include "drd_settings.h"
#include "drd_tool.h"

#include <KConfigGroup>

namespace Valgrind
{

DrdConfigPage::DrdConfigPage(QWidget* parent)
    : ConfigPage(parent)
    , ui(new Ui::DrdConfigPage())
{
    ui->setupUi(this);

    connectToChanged(ui->joinListVol);
    connectToChanged(ui->segmentMergingInterval);

    connectToChanged(ui->checkStackVar);
    connectToChanged(ui->firstRaceOnly);
    connectToChanged(ui->freeIsWrite);
    connectToChanged(ui->reportSignalUnlocked);
    connectToChanged(ui->segmentMerging);
    connectToChanged(ui->showConflSeg);
    connectToChanged(ui->showStackUsage);
    connectToChanged(ui->ignoreThreadCreation);
    connectToChanged(ui->showInstructionPointer);

    connectToChanged(ui->traceAlloc);
    connectToChanged(ui->traceBarrier);
    connectToChanged(ui->traceCond);
    connectToChanged(ui->traceForkJoin);
    connectToChanged(ui->traceHb);
    connectToChanged(ui->traceMutex);
    connectToChanged(ui->traceRwlock);
    connectToChanged(ui->traceSemaphore);

    ui->joinListVolLabel->setToolTip(ui->joinListVol->toolTip());
    ui->segmentMergingIntervalLabel->setToolTip(ui->segmentMergingInterval->toolTip());
}

DrdConfigPage::~DrdConfigPage() = default;

QString DrdConfigPage::title() const
{
    return DrdTool::self()->name();
}

QIcon DrdConfigPage::icon() const
{
    return QIcon();
}

void DrdConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    QSignalBlocker blocker(this);
    DrdSettings settings;
    settings.load(cfg);

    ui->joinListVol->setValue(settings.joinListVol);
    ui->segmentMergingInterval->setValue(settings.segmentMergingInterval);

    ui->checkStackVar->setChecked(settings.checkStackVar);
    ui->firstRaceOnly->setChecked(settings.firstRaceOnly);
    ui->freeIsWrite->setChecked(settings.freeIsWrite);
    ui->reportSignalUnlocked->setChecked(settings.reportSignalUnlocked);
    ui->segmentMerging->setChecked(settings.segmentMerging);
    ui->showConflSeg->setChecked(settings.showConflSeg);
    ui->showStackUsage->setChecked(settings.showStackUsage);
    ui->ignoreThreadCreation->setChecked(settings.ignoreThreadCreation);
    ui->showInstructionPointer->setChecked(settings.showInstructionPointer);

    ui->traceAlloc->setChecked(settings.traceAlloc);
    ui->traceBarrier->setChecked(settings.traceBarrier);
    ui->traceCond->setChecked(settings.traceCond);
    ui->traceForkJoin->setChecked(settings.traceForkJoin);
    ui->traceHb->setChecked(settings.traceHb);
    ui->traceMutex->setChecked(settings.traceMutex);
    ui->traceRwlock->setChecked(settings.traceRwlock);
    ui->traceSemaphore->setChecked(settings.traceSemaphore);
}

void DrdConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    DrdSettings settings;

    settings.joinListVol = ui->joinListVol->value();
    settings.segmentMergingInterval = ui->segmentMergingInterval->value();

    settings.checkStackVar = ui->checkStackVar->isChecked();
    settings.firstRaceOnly = ui->firstRaceOnly->isChecked();
    settings.freeIsWrite = ui->freeIsWrite->isChecked();
    settings.reportSignalUnlocked = ui->reportSignalUnlocked->isChecked();
    settings.segmentMerging = ui->segmentMerging->isChecked();
    settings.showConflSeg = ui->showConflSeg->isChecked();
    settings.showStackUsage = ui->showStackUsage->isChecked();
    settings.ignoreThreadCreation = ui->ignoreThreadCreation->isChecked();
    settings.showInstructionPointer = ui->showInstructionPointer->isChecked();

    settings.traceAlloc = ui->traceAlloc->isChecked();
    settings.traceBarrier = ui->traceBarrier->isChecked();
    settings.traceCond = ui->traceCond->isChecked();
    settings.traceForkJoin = ui->traceForkJoin->isChecked();
    settings.traceHb = ui->traceHb->isChecked();
    settings.traceMutex = ui->traceMutex->isChecked();
    settings.traceRwlock = ui->traceRwlock->isChecked();
    settings.traceSemaphore = ui->traceSemaphore->isChecked();

    settings.save(cfg);
}

DrdConfigPageFactory::DrdConfigPageFactory()
{
}

KDevelop::LaunchConfigurationPage* DrdConfigPageFactory::createWidget(QWidget* parent)
{
    return new DrdConfigPage(parent);
}

}
