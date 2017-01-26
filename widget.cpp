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

#include "debug.h"
#include "ijob.h"
#include "plugin.h"
#include "widget.h"

#include "cachegrind/cachegrindmodel.h"
#include "cachegrind/cachegrindview.h"
#include "callgrind/callgrindmodel.h"
#include "callgrind/callgrindview.h"
#include "massif/massifmodel.h"
#include "massif/massifview.h"

#include <klocalizedstring.h>

namespace valgrind
{

class ViewFactoryPrivate
{
public:
    static IView* make(Model* m);
};

IView* ViewFactoryPrivate::make(Model* m)
{
    if (dynamic_cast<MassifModel*>(m))
        return new MassifView();

    if (dynamic_cast<CachegrindModel*>(m))
        return new CachegrindView();

    if (dynamic_cast<CallgrindModel*>(m))
        return new CallgrindView();

    qCDebug(KDEV_VALGRIND) << "view not yet implemented";
    return nullptr;
}

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
    connect(this, &Widget::tabCloseRequested, this, &Widget::destroyRequestedTab);
    connect(plugin, &Plugin::newModel, this, &Widget::newModel);
}

Plugin* Widget::plugin() const
{
    return m_plugin;
}

void Widget::newModel(Model* model)
{
    IJob* job;

    job = model->job();
    if (!job)
        return;

    IView* w = ViewFactoryPrivate::make(model);
    if(!w)
        return;

    w->setModel(model);
    addTab(dynamic_cast<QWidget*>(w), QStringLiteral("%1 (%2)").arg(job->target()).arg(job->tool()));
    setCurrentWidget(dynamic_cast<QWidget*>(w));
    setMovable(true);
}

void Widget::destroyRequestedTab(int index)
{
    IView* view = dynamic_cast<IView*>(widget(index));

    // kill the job if it's still running
    if (view) {
        Model* model = dynamic_cast<IView*>(widget(index))->model();
        if (model) {
            if (model->job())
                model->job()->doKill();
            delete model;
        }
    }
    removeTab(index);
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
