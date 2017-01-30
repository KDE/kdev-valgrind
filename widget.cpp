/* This file is part of KDevelop
   Copyright 2007-2008 Hamish Rodda <rodda@kde.org>
   Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
   Copyright 2016 Anton Anikin <anton.anikin@htower.ru>

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

#include "widget.h"

#include "debug.h"
#include "interfaces/iview.h"
#include "plugin.h"

#include <klocalizedstring.h>

namespace valgrind
{

Widget::Widget(Plugin* plugin, QWidget* parent)
    : QTabWidget(parent)
    , m_plugin(plugin)
{
    setWindowIcon(QIcon::fromTheme("fork"));
    setWindowTitle(i18n("Valgrind Output"));

    setWhatsThis(i18n("<b>Valgrind</b><p>Shows the output of valgrind. Valgrind detects:<br/>"
                      "use of uninitialized memory;<br/>"
                      "reading/writing memory after it has been free'd;<br/>"
                      "reading/writing off the end of malloc'd blocks;<br/>"
                      "reading/writing inappropriate areas on the stack;<br/>"
                      "memory leaks &mdash; where pointers to malloc'd blocks are lost forever;<br/>"
                      "passing of uninitialised and/or unaddressable memory to system calls;<br/>"
                      "mismatched use of malloc/new/new [] vs free/delete/delete [];<br/>"
                      "some abuses of the POSIX pthread API.</p>"));

    setTabsClosable(true);
    connect(plugin, &Plugin::addView, this, &Widget::addView);

    connect(this, &Widget::tabCloseRequested, this, [this](int index) {
        delete widget(index);
        removeTab(index);
    });
}

Plugin* Widget::plugin() const
{
    return m_plugin;
}

void Widget::addView(IView* view)
{
    Q_ASSERT(view);

    addTab(dynamic_cast<QWidget*>(view), QStringLiteral("FIXME"));
    setCurrentWidget(dynamic_cast<QWidget*>(view));
    setMovable(true);
}

void Widget::resizeEvent(QResizeEvent* event)
{
    for (int i = 0; i < this->count(); ++i) {
        //notify child size has changed
        IView* view = dynamic_cast<IView*>(this->widget(i));
        if (view)
            view->WidgetContainerResizeEvent(event);
    }
}

}
