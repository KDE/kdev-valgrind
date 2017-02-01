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

    ui->extraParameters->setText(MassifSettings::extraParameters(cfg));
    ui->depth->setValue(MassifSettings::snapshotTreeDepth(cfg));
    ui->threshold->setValue(MassifSettings::threshold(cfg));
    ui->peakInaccuracy->setValue(MassifSettings::peakInaccuracy(cfg));
    ui->maxSnapshots->setValue(MassifSettings::maximumSnapshots(cfg));
    ui->snapshotFreq->setValue(MassifSettings::detailedSnapshotsFrequency(cfg));
    ui->timeUnit->setCurrentIndex(MassifSettings::timeUnit(cfg));
    ui->profileHeap->setChecked(MassifSettings::profileHeap(cfg));
    ui->profileStack->setChecked(MassifSettings::profileStack(cfg));
    ui->launchMassifVisualizer->setChecked(MassifSettings::launchVisualizer(cfg));
}

void MassifConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    MassifSettings::setExtraParameters(cfg, ui->extraParameters->text());
    MassifSettings::setSnapshotTreeDepth(cfg, ui->depth->value());
    MassifSettings::setThreshold(cfg, ui->threshold->value());
    MassifSettings::setPeakInaccuracy(cfg, ui->peakInaccuracy->value());
    MassifSettings::setMaximumSnapshots(cfg, ui->maxSnapshots->value());
    MassifSettings::setDetailedSnapshotsFrequency(cfg, ui->snapshotFreq->value());
    MassifSettings::setTimeUnit(cfg, ui->timeUnit->currentIndex());
    MassifSettings::setProfileHeap(cfg, ui->profileHeap->isChecked());
    MassifSettings::setProfileStack(cfg, ui->profileStack->isChecked());
    MassifSettings::setLaunchVisualizer(cfg, ui->launchMassifVisualizer->isChecked());
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
