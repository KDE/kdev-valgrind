/* This file is part of KDevelop
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>

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

#include <kconfiggroup.h>
#include "debug.h"
#include "genericconfigpage.h"
#include "plugin.h"

#include "ui_genericconfig.h"

namespace valgrind
{

GenericConfigPage::GenericConfigPage(QWidget *parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::GenericConfig();
    ui->setupUi(this);

    connect(ui->valgrindExecutable, &QLineEdit::textChanged, this, &GenericConfigPage::changed);
    connect(ui->valgrindParameters, &QLineEdit::textEdited, this, &GenericConfigPage::changed);
    connect(ui->limitErrors, &QCheckBox::toggled, this, &GenericConfigPage::changed);
    connect(ui->maxStackSize, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &GenericConfigPage::changed);
    connect(ui->stackDepth, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &GenericConfigPage::changed);

    QStringList tools;

    tools << "memcheck";
    tools << "massif";
    tools << "cachegrind";
    tools << "callgrind";

    /*
    ** Unimplemented tools: See launcher.cpp to activate their configuration tab
    */
    // tools << "helgrind";

    ui->currentTool->addItems(tools);

    connect(ui->currentTool, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &GenericConfigPage::changed);
}

GenericConfigPage::~GenericConfigPage(void)
{}

QIcon GenericConfigPage::icon() const
{
    return QIcon::fromTheme("fork");
}

void GenericConfigPage::loadFromConfiguration(const KConfigGroup &cfg, KDevelop::IProject *)
{
    bool wasBlocked = signalsBlocked();
    blockSignals(true);

    ui->valgrindExecutable->setText( cfg.readEntry("Valgrind Executable", QString("/usr/bin/valgrind")) );
    ui->valgrindParameters->setText(cfg.readEntry("Valgrind Arguments", ""));
    ui->stackDepth->setValue(cfg.readEntry("Stackframe Depth", 12));
    ui->maxStackSize->setValue(cfg.readEntry("Maximum Stackframe Size", 2000000));
    ui->limitErrors->setChecked(cfg.readEntry("Limit Errors", true));

    int tool_index;
    QString toolname;
    toolname = cfg.readEntry("Current Tool", "memcheck");
    tool_index = ui->currentTool->findText(toolname);
    ui->currentTool->setCurrentIndex(tool_index);

    blockSignals(wasBlocked);
}

void GenericConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject *) const
{
    cfg.writeEntry("Valgrind Executable", ui->valgrindExecutable->text());
    cfg.writeEntry("Valgrind Arguments", ui->valgrindParameters->text());
    cfg.writeEntry("Stackframe Depth", ui->stackDepth->value());
    cfg.writeEntry("Maximum Stackframe Size", ui->maxStackSize->value());
    cfg.writeEntry("Limit Errors", ui->limitErrors->isChecked());
    cfg.writeEntry("Current Tool", ui->currentTool->currentText());
    //useless ?
    emit newCurrentTool(ui->currentTool->currentText());
}

QString GenericConfigPage::title() const
{
    return i18n("Global settings");
}

GenericConfigPageFactory::GenericConfigPageFactory()
{
}

GenericConfigPageFactory::~GenericConfigPageFactory()
{
}

KDevelop::LaunchConfigurationPage* GenericConfigPageFactory::createWidget(QWidget * parent)
{
    return new GenericConfigPage(parent);
}

}
