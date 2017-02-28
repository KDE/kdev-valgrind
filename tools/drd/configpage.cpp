/* This file is part of KDevelop
 * Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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


#include "configpage.h"
#include "ui_configpage.h"

#include "settings.h"
#include "tool.h"

#include <KConfigGroup>

namespace Valgrind
{

namespace DRD
{

ConfigPage::ConfigPage(QWidget* parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::ConfigPage();
    ui->setupUi(this);

    connect(ui->joinListVol, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ConfigPage::changed);
    connect(ui->segmentMergingInterval, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ConfigPage::changed);

    connect(ui->checkStackVar, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->firstRaceOnly, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->freeIsWrite, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->reportSignalUnlocked, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->segmentMerging, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->showConflSeg, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->showStackUsage, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->ignoreThreadCreation, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->showInstructionPointer, &QCheckBox::toggled, this, &ConfigPage::changed);

    connect(ui->traceAlloc, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->traceBarrier, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->traceCond, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->traceForkJoin, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->traceHb, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->traceMutex, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->traceRwlock, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->traceSemaphore, &QCheckBox::toggled, this, &ConfigPage::changed);
}

ConfigPage::~ConfigPage()
{
    delete ui;
}

QString ConfigPage::title() const
{
    return Tool::self()->name();
}

QIcon ConfigPage::icon() const
{
    return QIcon();
}

void ConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    QSignalBlocker blocker(this);
    Settings settings;
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

void ConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    Settings settings;

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

ConfigPageFactory::ConfigPageFactory()
{
}

ConfigPageFactory::~ConfigPageFactory()
{
}

KDevelop::LaunchConfigurationPage* ConfigPageFactory::createWidget(QWidget* parent)
{
    return new ConfigPage(parent);
}

}

}
