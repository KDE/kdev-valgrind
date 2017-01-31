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

#include <KConfigGroup>

namespace valgrind
{

MassifConfigPage::MassifConfigPage(QWidget* parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::MassifConfig();
    ui->setupUi(this);

    connect(ui->extraParameters, &QLineEdit::textEdited, this, &MassifConfigPage::changed);
    connect(ui->launchMassifVisualizer, &QCheckBox::toggled, this, &MassifConfigPage::changed);
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

QString MassifConfigPage::title() const
{
    return i18n("Massif");
}

QIcon MassifConfigPage::icon() const
{
    return QIcon::fromTheme("fork");
}

void MassifConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    QSignalBlocker blocker(this);

    ui->extraParameters->setText(cfg.readEntry("Massif Extra Parameters", ""));
    ui->launchMassifVisualizer->setChecked(cfg.readEntry("Massif Launch Visualizer", false));
    ui->depth->setValue(cfg.readEntry("Massif Snapshot Tree Depth", 30));
    ui->threshold->setValue(cfg.readEntry("Massif Threshold", 1));
    ui->peakInaccuracy->setValue(cfg.readEntry("Massif Peak Inaccuracy", 1));
    ui->maxSnapshots->setValue(cfg.readEntry("Massif Maximum Snapshots", 100));
    ui->snapshotFreq->setValue(cfg.readEntry("Massif Detailed Snapshots Frequency", 10));
    ui->timeUnit->setCurrentIndex(cfg.readEntry("Massif Time Unit", 0));
    ui->profileHeap->setChecked(cfg.readEntry("Massif Profile Heap", true));
    ui->profileStack->setChecked(cfg.readEntry("Massif Profile Stack", false));
}

void MassifConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    cfg.writeEntry("Massif Extra Parameters", ui->extraParameters->text());
    cfg.writeEntry("Massif Launch Visualizer", ui->launchMassifVisualizer->isChecked());
    cfg.writeEntry("Massif Snapshot Tree Depth", ui->depth->value());
    cfg.writeEntry("Massif Threshold", ui->threshold->value());
    cfg.writeEntry("Massif Peak Inaccuracy", ui->peakInaccuracy->value());
    cfg.writeEntry("Massif Maximum Snapshots", ui->maxSnapshots->value());
    cfg.writeEntry("Massif Detailed Snapshots Frequency", ui->snapshotFreq->value());
    cfg.writeEntry("Massif Time Unit", ui->timeUnit->currentIndex());
    cfg.writeEntry("Massif Profile Heap", ui->profileHeap->isChecked());
    cfg.writeEntry("Massif Profile Stack", ui->profileStack->isChecked());
}

MassifConfigPageFactory::MassifConfigPageFactory()
{
}

MassifConfigPageFactory::~MassifConfigPageFactory()
{
}

KDevelop::LaunchConfigurationPage* MassifConfigPageFactory::createWidget(QWidget* parent)
{
    return new MassifConfigPage(parent);
}

}
