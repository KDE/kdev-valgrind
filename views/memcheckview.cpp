/* This file is part of KDevelop
 *  Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 *  Copyright 2008 Hamish Rodda <rodda@kde.org>

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

#include "memcheckview.h"

#include <QApplication>

#include "statjob.h"

#include <kdebug.h>
#include "debug.h"
#include <interfaces/icore.h>
#include <interfaces/idocumentcontroller.h>

#include "memcheckitemsimpl.h"
#include "memcheckmodel.h"

namespace valgrind
{
MemcheckView::MemcheckView()
{
    connect(this, SIGNAL(activated(QModelIndex)), SLOT(openDocument(QModelIndex)));
}

MemcheckView::~MemcheckView() {}

void MemcheckView::setModel(valgrind::Model * m)
{
    QTreeView::setModel(m->getQAbstractItemModel());
}

valgrind::Model * MemcheckView::model(void)
{
    return dynamic_cast<valgrind::Model *>(QTreeView::model());
}

void MemcheckView::openDocument(const QModelIndex & index)
{
    if (valgrind::MemcheckFrame* frame = dynamic_cast<valgrind::MemcheckFrame*>(static_cast<valgrind::MemcheckModel*>(model())->itemForIndex(index))) {
        QUrl doc = frame->url();
        if (doc.isValid() && StatJob::jobExists(doc, qApp->activeWindow())) {
            KDevelop::ICore::self()->documentController()->openDocument(doc, KTextEditor::Cursor(qMax(0, frame->line - 1), 0));
        }
    }
}

}

#include "memcheckview.moc"
