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

#include "debug.h"
#include "valgrindgenericconfigpage.h"
#include "valgrindplugin.h"

#include "ui_valgrindgenericconfig.h"

ValgrindGenericConfigPage::ValgrindGenericConfigPage(ValgrindPlugin *plugin, QWidget *parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::ValgrindGenericConfig();
    ui->setupUi(this);

    connect( ui->valgrindExecutable, SIGNAL(textChanged(QString)), SIGNAL(changed()) );
    connect( ui->valgrindExecutable, SIGNAL(urlSelected(QUrl)), SIGNAL(changed()) );
    connect( ui->valgrindParameters, SIGNAL(textEdited(QString)), SIGNAL(changed()) );
    connect( ui->limitErrors, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->maxStackSize, SIGNAL(valueChanged(int)), SIGNAL(changed()) );
    connect( ui->stackDepth, SIGNAL(valueChanged(int)), SIGNAL(changed()) );

    QStringList	tools;

    tools << "memcheck";

    /*
    ** Unimplemented tools: See valgrindlauncher.cpp to activate their configuration tab
    */
    // tools << "cachegrind";
    // tools << "callgrind";
    // tools << "helgrind";
    // tools << "massif";

    ui->currentTool->addItems(tools);

    connect( ui->currentTool, SIGNAL(currentIndexChanged(int)), SIGNAL(changed()) );
    connect( this, SIGNAL(newCurrentTool(QString)), plugin, SLOT(updateCurrentTool(QString)) );
}

ValgrindGenericConfigPage::~ValgrindGenericConfigPage(void)
{}

QIcon ValgrindGenericConfigPage::icon() const
{
    return QIcon("fork");
}

void ValgrindGenericConfigPage::loadFromConfiguration(const KConfigGroup &cfg, KDevelop::IProject *)
{
    bool wasBlocked = signalsBlocked();
    blockSignals(true);

    ui->valgrindExecutable->setUrl( cfg.readEntry( "Valgrind Executable", QUrl( "/usr/bin/valgrind" ) ) );
    ui->valgrindParameters->setText( cfg.readEntry( "Valgrind Arguments", "" ) );
    ui->stackDepth->setValue( cfg.readEntry( "Stackframe Depth", 12 ) );
    ui->maxStackSize->setValue( cfg.readEntry( "Maximum Stackframe Size", 2000000 ) );
    ui->limitErrors->setChecked( cfg.readEntry( "Limit Errors", true ) );

    int	tool_index;
    QString	toolname;
    toolname = cfg.readEntry( "Current Tool", "memcheck" );
    tool_index = ui->currentTool->findText(toolname);
    ui->currentTool->setCurrentIndex(tool_index);

    blockSignals(wasBlocked);
}

void ValgrindGenericConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject *) const
{
    cfg.writeEntry( "Valgrind Executable", ui->valgrindExecutable->url() );
    cfg.writeEntry( "Valgrind Arguments", ui->valgrindParameters->text() );
    cfg.writeEntry( "Stackframe Depth", ui->stackDepth->value() );
    cfg.writeEntry( "Maximum Stackframe Size", ui->maxStackSize->value() );
    cfg.writeEntry( "Limit Errors", ui->limitErrors->isChecked() );
    cfg.writeEntry( "Current Tool", ui->currentTool->currentText() );

    emit newCurrentTool( ui->currentTool->currentText() );
}

QString	ValgrindGenericConfigPage::title() const
{
    return i18n("Global settings");
}

// The factory
ValgrindGenericConfigPageFactory::ValgrindGenericConfigPageFactory(ValgrindPlugin * plugin)
    : m_plugin(plugin)
{}

ValgrindGenericConfigPageFactory::~ValgrindGenericConfigPageFactory()
{}

KDevelop::LaunchConfigurationPage* ValgrindGenericConfigPageFactory::createWidget(QWidget * parent)
{
    return new ValgrindGenericConfigPage(m_plugin, parent);
}
