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

#include "massifview.h"

#include <QApplication>
#include <QString>

#include "debug.h"

#include <interfaces/icore.h>
#include <interfaces/idocumentcontroller.h>

#include "massifitem.h"
#include "statjob.h"

namespace valgrind
{
MassifView::MassifView()
{
    connect(this, SIGNAL(activated(QModelIndex)), SLOT(openDocument(QModelIndex)));
}

MassifView::~MassifView(void) {}

void MassifView::setModel(valgrind::Model * m)
{
    QTreeView::setModel(m->getQAbstractItemModel());
}

valgrind::Model * MassifView::model(void)
{
    return dynamic_cast<valgrind::Model *>(QTreeView::model());
}

void MassifView::openDocument(const QModelIndex & index)
{
    if (valgrind::MassifItem* item = static_cast<valgrind::MassifItem*>(index.internalPointer())) {
        QUrl doc = item->url();
        if (doc.isValid() && StatJob::jobExists(doc, qApp->activeWindow())) {
            KDevelop::ICore::self()->documentController()->openDocument(doc, KTextEditor::Cursor(qMax(0, item->getLine() - 1), 0));
        }
    }
}

}

