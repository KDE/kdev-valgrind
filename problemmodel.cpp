/* This file is part of KDevelop

   Copyright 2017 Anton Anikin <anton@anikin.xyz>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "problemmodel.h"

#include "itool.h"
#include "plugin.h"
#include "utils.h"

#include <interfaces/icore.h>
#include <interfaces/ilanguagecontroller.h>
#include <shell/problemmodelset.h>

#include <klocalizedstring.h>

namespace Valgrind
{

inline KDevelop::ProblemModelSet* problemModelSet()
{
    return KDevelop::ICore::self()->languageController()->problemModelSet();
}

inline QString problemModelId() { return QStringLiteral("Valgrind"); }

ProblemModel::ProblemModel()
    : KDevelop::ProblemModel(Plugin::self())
{
    setFeatures(CanDoFullUpdate |
                ScopeFilter |
                SeverityFilter |
                Grouping |
                CanByPassScopeFilter |
                ShowSource);

    reset(nullptr);
    problemModelSet()->addModel(problemModelId(), i18n("Valgrind"), this);
}

ProblemModel::~ProblemModel()
{
    problemModelSet()->removeModel(problemModelId());
}

void ProblemModel::reset(const ITool* tool)
{
    m_tool = tool;
    clearProblems();

    QString toolName = tool ? tool->name() : QStringLiteral("Valgrind");
    setFullUpdateTooltip(i18nc("@info:tooltip", "Re-Run %1 Analysis for Current Launch Configuration", toolName));
}

void ProblemModel::show()
{
    problemModelSet()->showModel(problemModelId());
}

void ProblemModel::forceFullUpdate()
{
    if (m_tool && !Plugin::self()->isRunning()) {
        Plugin::self()->executeDefaultLaunch(m_tool->id());
    }
}

}
