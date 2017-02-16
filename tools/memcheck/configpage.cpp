/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
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

#include "debug.h"
#include "settings.h"

#include <kconfiggroup.h>

#include <QMenu>

namespace Valgrind
{

namespace Memcheck
{

ConfigPage::ConfigPage(QWidget* parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::ConfigPage();
    ui->setupUi(this);

    connect(ui->leakResolution, &QComboBox::currentTextChanged, this, &ConfigPage::changed);
    connect(ui->keepStacktraces, &QComboBox::currentTextChanged, this, &ConfigPage::changed);
    connect(ui->freelistVol, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ConfigPage::changed);
    connect(ui->freelistBigBlocks, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ConfigPage::changed);
    connect(ui->extraParameters, &QLineEdit::textEdited, this, &ConfigPage::changed);

    connect(ui->undefValueErrors, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->showMismatchedFrees, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->partialLoadsOk, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->trackOrigins, &QCheckBox::toggled, this, &ConfigPage::changed);
    connect(ui->expensiveDefinednessChecks, &QCheckBox::toggled, this, &ConfigPage::changed);

    connect(ui->showInstructionPointer, &QCheckBox::toggled, this, &ConfigPage::changed);

    static const QStringList leakKinds = {
        QStringLiteral("definite"),
        QStringLiteral("possible"),
        QStringLiteral("indirect"),
        QStringLiteral("reachable")
    };
    setupMenuButton(ui->showLeakKinds, leakKinds);

    static const QStringList heuristics = {
        QStringLiteral("stdstring"),
        QStringLiteral("length64"),
        QStringLiteral("newarray"),
        QStringLiteral("multipleinheritance")
    };
    setupMenuButton(ui->leakCheckHeuristics, heuristics);
}

ConfigPage::~ConfigPage()
{
    delete ui;
}

QString ConfigPage::title() const
{
    return i18n("Memcheck");
}

QIcon ConfigPage::icon() const
{
    return QIcon::fromTheme(QStringLiteral("fork"));
}

void ConfigPage::loadFromConfiguration(const KConfigGroup& cfg, KDevelop::IProject*)
{
    QSignalBlocker blocker(this);
    Settings settings;
    settings.load(cfg);

    ui->leakResolution->setCurrentText(settings.leakResolution);
    updateMenuButton(ui->showLeakKinds, settings.showLeakKinds);
    updateMenuButton(ui->leakCheckHeuristics, settings.leakCheckHeuristics);
    ui->keepStacktraces->setCurrentText(settings.keepStacktraces);
    ui->freelistVol->setValue(settings.freelistVol);
    ui->freelistBigBlocks->setValue(settings.freelistBigBlocks);
    ui->extraParameters->setText(settings.extraParameters);

    ui->undefValueErrors->setChecked(settings.undefValueErrors);
    ui->showMismatchedFrees->setChecked(settings.showMismatchedFrees);
    ui->partialLoadsOk->setChecked(settings.partialLoadsOk);
    ui->trackOrigins->setChecked(settings.trackOrigins);
    ui->expensiveDefinednessChecks->setChecked(settings.expensiveDefinednessChecks);

    ui->showInstructionPointer->setChecked(settings.showInstructionPointer);
}

void ConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject*) const
{
    Settings settings;

    settings.leakResolution = ui->leakResolution->currentText();
    settings.showLeakKinds = ui->showLeakKinds->text().trimmed().remove(QChar(' '));
    settings.leakCheckHeuristics = ui->leakCheckHeuristics->text().trimmed().remove(QChar(' '));
    settings.keepStacktraces = ui->keepStacktraces->currentText();
    settings.freelistVol = ui->freelistVol->value();
    settings.freelistBigBlocks = ui->freelistBigBlocks->value();
    settings.extraParameters = ui->extraParameters->text();

    settings.undefValueErrors = ui->undefValueErrors->isChecked();
    settings.showMismatchedFrees = ui->showMismatchedFrees->isChecked();
    settings.partialLoadsOk = ui->partialLoadsOk->isChecked();
    settings.trackOrigins = ui->trackOrigins->isChecked();
    settings.expensiveDefinednessChecks = ui->expensiveDefinednessChecks->isChecked();

    settings.showInstructionPointer = ui->showInstructionPointer->isChecked();

    settings.save(cfg);
}

QString selectedItemsText(QMenu* menu)
{
    Q_ASSERT(menu);

    QStringList selected;
    for (auto action : menu->actions()) {
        if (action->isChecked()) {
            selected += action->text();
        }
    }

    if (selected.isEmpty()) {
        return QStringLiteral("none");
    }

    if (selected.size() == menu->actions().size()) {
        return QStringLiteral("all");
    }


    return selected.join(QStringLiteral(", "));
}

void ConfigPage::setupMenuButton(QPushButton* button, const QStringList& items)
{
    Q_ASSERT(button);

    auto menu = new QMenu(button);
    button->setMenu(menu);
    button->setStyleSheet(QStringLiteral("Text-align:left"));

    auto slot = [button, menu]() {
        button->setText(QChar(' ') + selectedItemsText(menu));
    };

    for (const QString& text : items) {
        auto action = new QAction(text, menu);
        action->setCheckable(true);
        action->setChecked(false);

        connect(action, &QAction::toggled, this, slot);
        connect(action, &QAction::toggled, this, &ConfigPage::changed);
        menu->addAction(action);
    }
    slot();
}

void ConfigPage::updateMenuButton(QPushButton* button, const QString& text)
{
    Q_ASSERT(button);

    auto enabled = text.split(QChar(','));
    for (auto action : button->menu()->actions()) {
        if (text == QStringLiteral("all")) {
            action->setChecked(true);
        } else if (text == QStringLiteral("none")) {
            action->setChecked(false);
        } else {
            action->setChecked(enabled.contains(action->text()));
        }
    }
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
