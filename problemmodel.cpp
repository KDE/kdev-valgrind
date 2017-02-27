/* This file is part of KDevelop

   Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

static const QString problemModelId = QStringLiteral("Valgrind");

ProblemModel::ProblemModel(Plugin* plugin)
    : KDevelop::ProblemModel(plugin)
    , m_plugin(plugin)
{
    setFeatures(CanDoFullUpdate |
                ScopeFilter |
                SeverityFilter |
                Grouping |
                CanByPassScopeFilter);

    reset(QString());
    problemModelSet()->addModel(problemModelId, i18n("Valgrind"), this);
}

ProblemModel::~ProblemModel()
{
    problemModelSet()->removeModel(problemModelId);
}

void ProblemModel::reset(const QString& launcherId)
{
    clearProblems();

    m_launcherId = launcherId;
    QString toolName = launcherId.isEmpty() ? QStringLiteral("Valgrind") : launcherId;
    setFullUpdateTooltip(i18nc("@info:tooltip", "Re-run %1 Analyze for Current Launch Configuration", toolName));
}

void ProblemModel::show()
{
    problemModelSet()->showModel(problemModelId);
}

void ProblemModel::forceFullUpdate()
{
    if (!m_launcherId.isEmpty() && !m_plugin->isRunning()) {
        m_plugin->executeDefaultLaunch(m_launcherId);
    }
}

}
