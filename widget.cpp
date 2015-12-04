// -*- Mode: C++/l; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 8; -*-

/* This file is part of KDevelop
 *  Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 *  Copyright 2007-2008 Hamish Rodda <rodda@kde.org>

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

#include "widget.h"

#include "debug.h"
#include <klocalizedstring.h>

#include "debug.h"
#include "plugin.h"
#include "job.h"
#include "imodel.h"
#include "cachegrindview.h"
#include "callgrindview.h"
#include "massifview.h"
#include "massifmodel.h"
#include "cachegrindmodel.h"
#include "callgrindmodel.h"

#include <QResizeEvent>

namespace valgrind
{

class ViewFactoryPrivate
{
public:
    static valgrind::IView * make(valgrind::Model * m);
};

valgrind::IView * ViewFactoryPrivate::make(valgrind::Model * m)
{
    if (dynamic_cast<valgrind::MassifModel *>(m))
        return new valgrind::MassifView();
    if (dynamic_cast<valgrind::CachegrindModel *>(m))
        return new valgrind::CachegrindView();
    if (dynamic_cast<valgrind::CallgrindModel *>(m))
        return new valgrind::CallgrindView();

    qCDebug(KDEV_VALGRIND) << "view not yet implemented";
    return NULL;
}

Widget::Widget(valgrind::Plugin * plugin, QWidget * parent)
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
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(destroyRequestedTab(int)));
    connect(plugin, SIGNAL(newModel(valgrind::Model*)), this, SLOT(newModel(valgrind::Model *)));
}

valgrind::Plugin * Widget::plugin() const
{
    return m_plugin;
}

void Widget::newModel(valgrind::Model * model)
{
    valgrind::Job * job;

    job = model->job();
    if (job)
    {
        valgrind::IView * w = ViewFactoryPrivate::make(model);
        if(!w)
            return;

        w->setModel(model);
        connect(job, SIGNAL(updateTabText(valgrind::Model *, const QString &)),
                this, SLOT(updateTabText(valgrind::Model *, const QString &)));
        addTab(dynamic_cast<QWidget *>(w), i18n("job scheduled"));
        setCurrentWidget(dynamic_cast<QWidget *>(w));
        setMovable(true);
    }
}

void Widget::destroyRequestedTab(int index)
{
    valgrind::IView * view = dynamic_cast<valgrind::IView *>(widget(index));

    // kill the job if it's still running
    if (view)
    {
        valgrind::Model * model = dynamic_cast<valgrind::IView *>(widget(index))->model();
        if (model)
        {
            if (model->job())
            {
                model->job()->doKill();
            }
            delete model;
        }
    }
    removeTab(index);
}

void Widget::updateTabText(valgrind::Model * model, const QString & text)
{
    for (int i = 0; i < count(); ++i)
    {
        valgrind::IView * view = dynamic_cast<valgrind::IView *>(widget(i));
        if (view && view->model() == model)
        {
            setTabText(i, text);
        }
    }
}

void Widget::resizeEvent( QResizeEvent *event )
{
    for (int i = 0; i < this->count(); ++i)
    {
        //notify child size has changed
        IView *view = dynamic_cast<IView *>(this->widget(i));
        if (view != NULL)
            view->WidgetContainerResizeEvent(event);
    }
}

}

