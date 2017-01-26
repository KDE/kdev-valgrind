/* This file is part of KDevelop
 * Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>

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

#include <KConfigGroup>

namespace valgrind
{

MassifConfigPage::MassifConfigPage(QWidget *parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::MassifConfig();
    ui->setupUi(this);
    connect(ui->massifParameters, &QLineEdit::textEdited, this, &MassifConfigPage::changed);
    connect(ui->launchMassifVisualizer, &QCheckBox::toggled, this, &MassifConfigPage::changed);
    connect(ui->massifVisualizerExecutable, &QLineEdit::textEdited, this, &MassifConfigPage::changed);
    connect(ui->depth, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MassifConfigPage::changed);
    connect(ui->threshold, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MassifConfigPage::changed);
    connect(ui->peakInaccuracy, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MassifConfigPage::changed);
    connect(ui->maxSnapshots, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MassifConfigPage::changed);
    connect(ui->snapshotFreq, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MassifConfigPage::changed);
    connect(ui->timeUnit, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MassifConfigPage::changed);
    connect(ui->profileHeap, &QCheckBox::toggled, this, &MassifConfigPage::changed);
    connect(ui->profileStack, &QCheckBox::toggled, this, &MassifConfigPage::changed);
}

void    MassifConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject *)
{
    bool wasBlocked = signalsBlocked();
    blockSignals(true);

    ui->massifParameters->setText(cfg.readEntry("Massif Arguments", ""));
    ui->launchMassifVisualizer->setChecked(cfg.readEntry("launchVisualizer", false));
    ui->massifVisualizerExecutable->setText(cfg.readEntry("visualizerExecutable", "/usr/bin/massif-visualizer"));
    ui->depth->setValue(cfg.readEntry("depth", 30));
    ui->threshold->setValue(cfg.readEntry("threshold", 1));
    ui->peakInaccuracy->setValue(cfg.readEntry("peakInaccuracy", 1));
    ui->maxSnapshots->setValue(cfg.readEntry("maxSnapshots", 100));
    ui->snapshotFreq->setValue(cfg.readEntry("snapshotFreq", 10));
    ui->timeUnit->setCurrentIndex(cfg.readEntry("timeUnit", 0));
    ui->profileHeap->setChecked(cfg.readEntry("profileHeap", true));
    ui->profileStack->setChecked(cfg.readEntry("profileStack", false));

    blockSignals(wasBlocked);
}

QIcon   MassifConfigPage::icon(void) const
{
    return QIcon::fromTheme("fork");
}

void    MassifConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject *) const
{
    cfg.writeEntry("Massif Arguments", ui->massifParameters->text());
    cfg.writeEntry("launchVisualizer", ui->launchMassifVisualizer->isChecked());
    cfg.writeEntry("visualizerExecutable", ui->massifVisualizerExecutable->text());
    cfg.writeEntry("depth", ui->depth->value());
    cfg.writeEntry("threshold", ui->threshold->value());
    cfg.writeEntry("peakInaccuracy", ui->peakInaccuracy->value());
    cfg.writeEntry("maxSnapshots", ui->maxSnapshots->value());
    cfg.writeEntry("snapshotsFreq", ui->snapshotFreq->value());
    cfg.writeEntry("timeUnit", ui->timeUnit->currentIndex());
    cfg.writeEntry("profileHeap", ui->profileHeap->isChecked());
    cfg.writeEntry("profileStack", ui->profileStack->isChecked());
}

QString MassifConfigPage::title(void) const
{
    return i18n("Massif");
}

// The factory
MassifConfigPageFactory::MassifConfigPageFactory(void)
{}

MassifConfigPageFactory::~MassifConfigPageFactory(void)
{}

KDevelop::LaunchConfigurationPage* MassifConfigPageFactory::createWidget(QWidget *parent)
{
    return new MassifConfigPage(parent);
}
}
