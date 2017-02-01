/* This file is part of KDevelop
 *  Copyright 2011 Sebastien Rannou <mxs@sbrk.org>
 *  Copyright 2008 Hamish Rodda <rodda@kde.org>
 *  Copyright 2011 Lucas Sarie <lucas.sarie@gmail.com>
 *  Copyright 2017 Anton Anikin <anton.anikin@htower.ru>

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

#include "view.h"

#include <QApplication>
#include <QHeaderView>

#include "statjob.h"

#include "debug.h"
#include <interfaces/icore.h>
#include <interfaces/idocumentcontroller.h>

#include "model.h"
#include "modelitem.h"

namespace valgrind
{

CachegrindView::CachegrindView()
{
    connect(this, &CachegrindView::activated, this, &CachegrindView::openDocument);
}

CachegrindView::~CachegrindView() {}

void CachegrindView::setModel(CachegrindModel* m)
{
    QTreeView::setModel(m);

    connect(m, &CachegrindModel::modelChanged, this, [this, m]() {
        if (m->columnCount()) {
            header()->setStretchLastSection(false);
            header()->setSectionResizeMode(0, QHeaderView::Stretch);
        }
    });
}

void CachegrindView::MousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event)

    qCDebug(KDEV_VALGRIND) << "Mouse pressed...";
//     QWidget::mousePressEvent(event);
}

void CachegrindView::openDocument(const QModelIndex & index)
{
    if (CachegrindItem* frame = dynamic_cast<CachegrindItem*>(static_cast<CachegrindModel*>(model())->itemForIndex(index)))
    {
        QUrl doc = frame->url();
        if (doc.isValid() && StatJob::jobExists(doc, qApp->activeWindow()))
        {
            KDevelop::ICore::self()->documentController()->openDocument(doc, KTextEditor::Cursor(qMax(0, 0), 0));
        }
    }
}
}

