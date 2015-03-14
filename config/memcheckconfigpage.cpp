/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
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


#include "memcheckconfigpage.h"

#include "ui_memcheckconfig.h"

#include <KConfigGroup>

namespace valgrind
{

MemcheckConfigPage::MemcheckConfigPage(QWidget *parent)
    : LaunchConfigurationPage(parent)
{
    ui = new Ui::MemcheckConfig();
    ui->setupUi(this);

    connect(ui->memcheckParameters, SIGNAL(textEdited(QString)), SIGNAL(changed()));
    connect(ui->freeListSize, SIGNAL(valueChanged(int)), SIGNAL(changed()));

    connect(ui->showReachable, SIGNAL(toggled(bool)), SIGNAL(changed()));
    //    connect( ui->trackOrigins, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect(ui->undefValueErrors, SIGNAL(toggled(bool)), SIGNAL(changed()));
    /*
    connect( ui->showInvalidFree, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showMismatchedFree, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showInvalidRead, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showInvalidWrite, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showInvalidJump, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showOverlap, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showInvalidMemPool, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showUninitCondition, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showUninitValue, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showSyscallParam, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showClientCheck, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showLeakDefinitelyLost, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showLeakIndirectlyLost, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showLeakStillReachable, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    connect( ui->showOthers, SIGNAL(toggled(bool)), SIGNAL(changed()) );
    */
}

QIcon MemcheckConfigPage::icon() const
{
    return QIcon::fromTheme("fork");
}

void MemcheckConfigPage::loadFromConfiguration(const KConfigGroup &cfg, KDevelop::IProject *)
{
    bool wasBlocked = signalsBlocked();
    blockSignals(true);

    ui->memcheckParameters->setText(cfg.readEntry("Memcheck Arguments", ""));
    ui->freeListSize->setValue(cfg.readEntry("Freelist Size", 10000000));

    ui->showReachable->setChecked(cfg.readEntry("Show Reachable", true));
    //    ui->trackOrigins->setChecked( cfg.readEntry( "Track Origins", false ) );
    ui->undefValueErrors->setChecked(cfg.readEntry("Undef Value Errors", true));

    // by default everything is enabled
    /*
    ui->showInvalidFree->setChecked( cfg.readEntry( "Show Invalid Free", true ) );
    ui->showMismatchedFree->setChecked( cfg.readEntry( "Show Mismatched Free", true ) );
    ui->showInvalidRead->setChecked( cfg.readEntry( "Show Invalid Read", true ) );
    ui->showInvalidWrite->setChecked( cfg.readEntry( "Show Invalid Write", true ) );
    ui->showInvalidJump->setChecked( cfg.readEntry( "Show Invalid Jump", true ) );
    ui->showOverlap->setChecked( cfg.readEntry( "Show Overlap", true ) );
    ui->showInvalidMemPool->setChecked( cfg.readEntry( "Show Invalid MemPool", true ) );
    ui->showUninitCondition->setChecked( cfg.readEntry( "Show Uninitialized Condition", true ) );
    ui->showUninitValue->setChecked( cfg.readEntry( "Show Uninitialized Value", true ) );
    ui->showSyscallParam->setChecked( cfg.readEntry( "Show Syscall Param", true ) );
    ui->showClientCheck->setChecked( cfg.readEntry( "Show Client Check", true ) );
    ui->showLeakDefinitelyLost->setChecked( cfg.readEntry( "Show Leak Definitely Lost", true ) );
    ui->showLeakIndirectlyLost->setChecked( cfg.readEntry( "Show Leak Indirectly Lost", true ) );
    ui->showLeakStillReachable->setChecked( cfg.readEntry( "Show Leak Still Reachable", true ) );
    ui->showOthers->setChecked( cfg.readEntry( "Show Others", true ) );
    */
    blockSignals(wasBlocked);
}

void MemcheckConfigPage::saveToConfiguration(KConfigGroup cfg, KDevelop::IProject *) const
{
    cfg.writeEntry("Memcheck Arguments", ui->memcheckParameters->text());
    cfg.writeEntry("Freelist Size", ui->freeListSize->value());

    cfg.writeEntry("Show Reachable", ui->showReachable->isChecked());
    //    cfg.writeEntry( "Track Origins", ui->trackOrigins->isChecked() );
    cfg.writeEntry("Undef Value Errors", ui->undefValueErrors->isChecked());
    /*
    cfg.writeEntry( "Show Invalid Free", ui->showInvalidFree->isChecked() );
    cfg.writeEntry( "Show Mismatched Free", ui->showMismatchedFree->isChecked() );
    cfg.writeEntry( "Show Invalid Read", ui->showInvalidRead->isChecked() );
    cfg.writeEntry( "Show Invalid Write", ui->showInvalidWrite->isChecked() );
    cfg.writeEntry( "Show Invalid Jump", ui->showInvalidJump->isChecked() );
    cfg.writeEntry( "Show Overlap", ui->showOverlap->isChecked() );
    cfg.writeEntry( "Show Invalid MemPool", ui->showInvalidMemPool->isChecked() );
    cfg.writeEntry( "Show Uninitialized Condition", ui->showUninitCondition->isChecked() );
    cfg.writeEntry( "Show Uninitialized Value", ui->showUninitValue->isChecked() );
    cfg.writeEntry( "Show Syscall Param", ui->showSyscallParam->isChecked() );
    cfg.writeEntry( "Show Client Check", ui->showClientCheck->isChecked() );
    cfg.writeEntry( "Show Leak Definitely Lost", ui->showLeakDefinitelyLost->isChecked() );
    cfg.writeEntry( "Show Leak Indirectly Lost", ui->showLeakIndirectlyLost->isChecked() );
    cfg.writeEntry( "Show Leak Still Reachable", ui->showLeakStillReachable->isChecked() );
    cfg.writeEntry( "Show Others", ui->showOthers->isChecked() );
    */
}

QString MemcheckConfigPage::title() const
{
    return i18n("Memcheck");
}

// The factory
MemcheckConfigPageFactory::MemcheckConfigPageFactory()
{}

MemcheckConfigPageFactory::~MemcheckConfigPageFactory()
{}

KDevelop::LaunchConfigurationPage* MemcheckConfigPageFactory::createWidget(QWidget * parent)
{
    return new valgrind::MemcheckConfigPage(parent);
}
}
