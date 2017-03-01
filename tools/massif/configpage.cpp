/* This file is part of KDevelop
 * Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
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

namespace Massif
{

ConfigPage::ConfigPage(QWidget* parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::ConfigPage();
    ui->setupUi(this);

    connect(ui->extraParameters, &QLineEdit::textEdited, this, &ConfigPage::changed);
    connect(ui->launchMassifVisualizer, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->depth, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ConfigPage::changed);
    connect(ui->threshold, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ConfigPage::changed);
    connect(ui->peakInaccuracy, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ConfigPage::changed);
    connect(ui->maxSnapshots, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ConfigPage::changed);
    connect(ui->snapshotFreq, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ConfigPage::changed);
    connect(ui->timeUnit, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &ConfigPage::changed);
    connect(ui->profileHeap, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->profileStack, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->pagesAsHeap, &QCheckBox::toggled, this, &ConfigPage::changed);

    connect(ui->profileStack, &QCheckBox::toggled, this, &ConfigPage::check);
    connect(ui->pagesAsHeap, &QCheckBox::toggled, this, &ConfigPage::check);
    check();

    ui->timeUnitLabel->setToolTip(ui->timeUnit->toolTip());
    ui->depthLabel->setToolTip(ui->depth->toolTip());
    ui->thresholdLabel->setToolTip(ui->threshold->toolTip());
    ui->peakInaccuracyLabel->setToolTip(ui->peakInaccuracy->toolTip());
    ui->maxSnapshotsLabel->setToolTip(ui->maxSnapshots->toolTip());
    ui->snapshotFreqLabel->setToolTip(ui->snapshotFreq->toolTip());
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

void ConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    Settings settings;

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

void ConfigPage::check()
{
    if (ui->profileStack->isChecked() && ui->pagesAsHeap->isChecked()) {
        ui->messageWidget->setVisible(true);
        return;
    }

    ui->messageWidget->setVisible(false);
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
