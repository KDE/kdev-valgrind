/* This file is part of KDevelop
   Copyright 2018 Anton Anikin <anton@anikin.xyz>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING. If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "menubutton.h"

#include <QMenu>

#include <KLocalizedString>

namespace Valgrind
{

MenuButton::MenuButton(QWidget* parent)
    : QPushButton(parent)
{
    setMenu(new QMenu(this));
}

QAction* MenuButton::addAction(const QString& text, const QString& data)
{
    auto action = new QAction(text, this);
    action->setData(data);
    action->setCheckable(true);
    action->setChecked(false);

    connect(action, &QAction::toggled, this, &MenuButton::updateValue);

    menu()->addAction(action);
    updateValue();

    return action;
}

QString MenuButton::value() const
{
    return m_value;
}

void MenuButton::setValue(const QString& value)
{
    if (m_value == value) {
        return;
    }

    const auto enabled = value.split(QLatin1Char(','));
    const auto actions = menu()->actions();

    for (auto action : actions) {
        QSignalBlocker blocker(action);

        if (value == QStringLiteral("all")) {
            action->setChecked(true);
        } else if (value == QStringLiteral("none")) {
            action->setChecked(false);
        } else {
            action->setChecked(enabled.contains(action->data().toString()));
        }
    }

    QSignalBlocker blocker(this);
    updateValue();
}

void MenuButton::updateValue()
{
    QStringList dataSelected;
    QStringList textSelected;

    const auto actions = menu()->actions();
    for (auto action : actions) {
        if (action->isChecked()) {
            dataSelected += action->data().toString();
            textSelected += action->text();
        }
    }

    if (dataSelected.isEmpty()) {
        m_value = QStringLiteral("none");
        setText(i18n("none"));
    } else if (dataSelected.size() == actions.size()) {
        m_value = QStringLiteral("all");
        setText(i18n("all"));
    } else {
        m_value = dataSelected.join(QLatin1Char(','));
        setText(textSelected.join(QLatin1String(", ")));
    }

    emit valueChanged(m_value);
}

}
