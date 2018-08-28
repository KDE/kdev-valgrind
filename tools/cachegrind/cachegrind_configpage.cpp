/* This file is part of KDevelop
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2011 Lionel Duc <lionel.data@gmail.com>
   Copyright 2017 Anton Anikin <anton@anikin.xyz>

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

#include "cachegrind_configpage.h"
#include "ui_cachegrind_configpage.h"

#include "cachegrind_config.h"
#include "cachegrind_tool.h"

#include <KConfigGroup>

namespace Valgrind
{

CachegrindConfigPage::CachegrindConfigPage(QWidget* parent)
    : ConfigPage(CachegrindTool::self()->name(), parent)
    , ui(new Ui::CachegrindConfigPage())
{
    ui->setupUi(this);

    init(new CachegrindConfig);
}

CachegrindConfigPage::~CachegrindConfigPage() = default;

void CachegrindConfigPage::check()
{
    ui->messageWidget->setVisible(!(ui->kcfg_cacheSim->isChecked() || ui->kcfg_branchSim->isChecked()));
}

}
