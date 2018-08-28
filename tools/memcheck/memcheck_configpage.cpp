/* This file is part of KDevelop
 * Copyright 2011 Mathieu Lornac <mathieu.lornac@gmail.com>
 * Copyright 2011 Damien Coppel <damien.coppel@gmail.com>
 * Copyright 2011 Lionel Duc <lionel.data@gmail.com>
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

#include "memcheck_configpage.h"
#include "ui_memcheck_configpage.h"

#include "debug.h"
#include "memcheck_config.h"
#include "memcheck_tool.h"

#include <QMenu>

#include <KLocalizedString>

namespace Valgrind
{

MemcheckConfigPage::MemcheckConfigPage(QWidget* parent)
    : ConfigPage(MemcheckTool::self()->name(), parent)
{
    Ui::MemcheckConfigPage ui;
    ui.setupUi(this);

    ui.kcfg_showLeakKinds->addAction(i18n("definite"), QStringLiteral("definite"));
    ui.kcfg_showLeakKinds->addAction(i18n("possible"), QStringLiteral("possible"));
    ui.kcfg_showLeakKinds->addAction(i18n("indirect"), QStringLiteral("indirect"));
    ui.kcfg_showLeakKinds->addAction(i18n("reachable"), QStringLiteral("reachable"));
    connect(ui.kcfg_showLeakKinds, &MenuButton::valueChanged, this, &MemcheckConfigPage::changed);

    // FIXME fix i18n text ?
    ui.kcfg_leakCheckHeuristics->addAction(i18n("stdstring"), QStringLiteral("stdstring"));
    ui.kcfg_leakCheckHeuristics->addAction(i18n("length64"), QStringLiteral("length64"));
    ui.kcfg_leakCheckHeuristics->addAction(i18n("newarray"), QStringLiteral("newarray"));
    ui.kcfg_leakCheckHeuristics->addAction(i18n("multipleinheritance"), QStringLiteral("multipleinheritance"));
    connect(ui.kcfg_leakCheckHeuristics, &MenuButton::valueChanged, this, &MemcheckConfigPage::changed);

    ui.kcfg_leakResolution->addItem(i18n("high"), QStringLiteral("high"));
    ui.kcfg_leakResolution->addItem(i18n("medium"), QStringLiteral("med"));
    ui.kcfg_leakResolution->addItem(i18n("low"), QStringLiteral("low"));

    ui.kcfg_keepStacktraces->addItem(i18n("alloc"), QStringLiteral("alloc"));
    ui.kcfg_keepStacktraces->addItem(i18n("free"), QStringLiteral("free"));
    ui.kcfg_keepStacktraces->addItem(i18n("alloc-and-free"), QStringLiteral("alloc-and-free"));
    ui.kcfg_keepStacktraces->addItem(i18n("alloc-then-free"), QStringLiteral("alloc-then-free"));
    ui.kcfg_keepStacktraces->addItem(i18n("none"), QStringLiteral("none"));

    init(new MemcheckConfig);
}

}
